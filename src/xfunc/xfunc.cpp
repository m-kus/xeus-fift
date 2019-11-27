#include <td/utils/port/path.h>

#include "xfunc.hpp"
#include "xfift/xfift.hpp"

using namespace xfift;
using namespace funC;
using namespace src;
using namespace td;

namespace xfift {

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
    }

    XResult XFunc::do_interpret(const std::string& expr) 
    {
        std::vector<std::string> func_names;
        std::string source = expr;
        
        try {
            resolve_includes(source);
            parse_expression(source, func_names);
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

    XToken XFunc::parse_token(const std::string& line, std::size_t cursor_pos) 
    {
        XToken token = xfift::parse_token(line, cursor_pos, " ~.", " \"(?");
        if (token.prev_char() == '~') {
            token.begin_pos--;
        }
        return token;
    }

    bool XFunc::code_complete(const XToken& token, std::vector<std::string>& matches) 
    {
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
        }
    }

    std::string XFunc::code_inspect(const XToken& token) {
        return "";
    }
}