#include <td/utils/port/path.h>

#include "xfunc.hpp"
#include "xfift/xfift.hpp"

using namespace xfift;
using namespace funC;
using namespace src;
using namespace td;

namespace xfift {

    const std::initializer_list<std::string> completion = {
        // helpers
        "#include", "print",
        // constants
        "main", "recv_internal", "recv_external",
        // keywords
        "int", "cell", "slice", "builder", "cont", "tuple",
        "return", "var", "repeat", "do", "while", "until", "if", "ifnot", "then", "else", "elseif", "elseifnot",
        "extern", "asm", "impure", "inline", "inline_ref", "method_id", "operator", "infix", "infixl", "infixr",
        // builtins
        "divmod", "moddiv", "muldivr", "muldiv", "muldivmod", "null?", "throw", "throw_if", "throw_unless", 
        "load_int", "load_uint", "preload_int", "preload_uint", "store_int", "store_uint", "load_bits", 
        "preload_bits", "int_at", "cell_at", "slice_at", "tuple_at", "at", "touch", "touch2", "dump", 
        "run_method0", "run_method1", "run_method2", "run_method3", "~divmod", "~moddiv", "~store_int", 
        "~store_uint", "~touch", "~touch2"
    };

    XFunc::XFunc()
        : completion_(completion)
    {
    }

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

        do_interpret("#include \"stdlib.fc\"");
        do_interpret("forall X -> () print(X a) impure asm \"s0 PRINT\";");
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

        std::copy(
            func_names.begin(), 
            func_names.end(), 
            std::inserter(completion_, completion_.begin()));
        
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
        // ~non_const_method | .const_method
        XToken token = parse_token(expr, cursor_pos, " ~.(", " \"?()");
        if (token.prev_char() == '~') {
            token.begin_pos--;
        }

        std::string prefix = token.str();
        if (!prefix.empty())
        {
            if (token.prev_char() == '"') {
                fs::path p{prefix};
                if (p.has_parent_path()) {
                    path_complete(p, matches);
                } else {
                    for (auto& dir : func_path_) {
                        path_complete(fs::path{dir} / p, matches);
                    }
                }
            } else {
                std::for_each(completion.begin(), completion.end(), [&](const std::string& x) {
                    if (std::equal(prefix.begin(), prefix.end(), x.begin())) {
                        matches.push_back(x);
                    }
                });
            }
        }
        
        return std::move(token);
    }

    XToken XFunc::code_inspect(const std::string& expr, std::size_t cursor_pos, std::string& tooltip) {
        XToken token = parse_token(expr, cursor_pos, "\n~.;=><(", "\n() ");
        if (token.prev_char() == '~') {
            token.begin_pos--;
        }
        
        sym::SymDef* sym_def = sym::lookup_symbol(token.str());
        if (sym_def) {
            XToken signature = parse_token(sym_def->loc.text, 0, "", "{");
            tooltip = "\n" + strip(signature.str());
        }

        return std::move(token);
    }
}
