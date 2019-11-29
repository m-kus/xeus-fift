#include <td/utils/port/path.h>

#include "xfunc.hpp"
#include "xfift/xfift.hpp"

using namespace xfift;
using namespace funC;
using namespace src;
using namespace td;

namespace xfift {

    const std::vector<std::string> keywords = {
        "main", "recv_internal", "recv_external", "#include",
        "int", "cell", "slice", "builder", "cont", "tuple",
        "return", "var", "repeat", "do", "while", "until", "if", "ifnot", "then", "else", "elseif", "elseifnot",
        "extern", "asm", "impure", "inline", "inline_ref", "method_id", "operator", "infix", "infixl", "infixr",
        "divmod ", "moddiv ", "muldivr ", "muldiv ", "muldivmod ", "null? ", "throw ", "throw_if ", "throw_unless ", 
        "load_int ", "load_uint ", "preload_int ", "preload_uint ", "store_int ", "store_uint ", "load_bits ", 
        "preload_bits ", "int_at", "cell_at ", "slice_at ", "tuple_at ", "at ", "touch ", "touch2 ", "dump ", 
        "run_method0 ", "run_method1 ", "run_method2 ", "run_method3", "~divmod ", "~moddiv ", "~store_int ", 
        "~store_uint ", "~touch ", "~touch2"
    };

    XFunc::~XFunc() 
    {
        reset_global_state();
    }

    void XFunc::configure() 
    {        
        const char* path = std::getenv("FUNCPATH");
        split(path ? path : "/usr/lib/func", ':', func_path_);

        auto r_current_dir = td::realpath(".");
        if (r_current_dir.is_ok()) {
            func_path_.push_back(r_current_dir.move_as_ok());
        }

        funC::define_keywords();
        funC::define_builtins();
        funC::verbosity = 5;

        do_interpret("#include \"stdlib.fc\"\n"
                     "forall X -> () print(X a) impure asm \"s0 PRINT\";");
    }

    XResult XFunc::do_interpret(const std::string& expr) 
    {
        std::vector<std::string> func_names;
        std::string source = expr;
        
        try {
            resolve_includes(source);
            parse_functions(source, func_names);
            force_main(source, func_names);
        } catch (const std::string& msg) {
            return XResult(msg);
        }
        
        std::stringstream ss(source);
        src::FileDescr fd{"stdin", true};
        std::string script_fif;

        try 
        {
            { 
                global_sym_guard sym_guard(func_names);
                funC::parse_source(&ss, &fd); 
            }
            if (std::find(func_names.begin(), func_names.end(), "main") != func_names.end()) {
                script_fif = generate_fift_script();
            }
        } catch (src::Fatal& fatal) {
            return XResult(fatal);
        } catch (src::ParseError& error) {
            return XResult(error);
        } catch (funC::UnifyError& unif_err) {
            return XResult(unif_err);
        }

        if (!script_fif.empty()) {
            XFift fift;
            fift.configure();
            XResult res = fift.do_interpret(script_fif);
            return res.code == 0 ? res.vm_result() : res;
        }

        return XResult();
    }

    XToken XFunc::code_complete(const std::string& expr, std::size_t cursor_pos, std::vector<std::string>& matches) 
    {
        // symbol subclass:
        // 1 = begins with . (a const method)
        // 2 = begins with ~ (a non-const method)
        // 0 = else
        XToken token = parse_token(expr, cursor_pos, " ~.(", " \"?()");
        if (token.prev_char() == '~') {
            token.begin_pos--;
        }

        std::string prefix = token.str();
        if (!prefix.empty() && token.prev_char() == '"') {
            fs::path p{prefix};
            if (p.has_parent_path()) {
                path_complete(p, matches);
            } else {
                for (auto& dir : func_path_) {
                    path_complete(fs::path{dir} / p, matches);
                }
            }
        } else {
            std::for_each(funC::glob_func.begin(), funC::glob_func.end(), [&](SymDef* x) {
                std::string func_name = x->name();
                if (prefix.empty() || std::equal(prefix.begin(), prefix.end(), func_name.begin())) {
                    matches.push_back(std::move(func_name));
                }
            });
            if (!prefix.empty()) {
                std::for_each(keywords.begin(), keywords.end(), [&](const std::string& x) {
                    if (std::equal(prefix.begin(), prefix.end(), x.begin())) {
                        matches.push_back(x);
                    }
                });
            }
        }
        
        return std::move(token);
    }

    XToken XFunc::code_inspect(const std::string& expr, std::size_t cursor_pos, std::string& tooltip) {
        XToken token = parse_token(expr, cursor_pos, "\n", "\n");

        std::vector<std::string> func_names;
        parse_functions(token.str(), func_names);
        
        if (func_names.size() == 1) {
            sym::SymDef* sym_def = sym::lookup_symbol(func_names[0], 2);
            if (sym_def) {
                tooltip = sym_def->loc.text;
            }
        }

        return std::move(token);
    }
}