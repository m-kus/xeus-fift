#include "xfunc.hpp"
#include "xfift/xfift.hpp"

using namespace xfift;
using namespace funC;
using namespace src;

namespace xfift {

    XFunc::~XFunc() {
        reset_global_state();
    }

    void XFunc::configure() {        
        funC::define_keywords();
        funC::define_builtins();
        funC::verbosity = 5;
    }

    XResult XFunc::do_interpret(const std::string& expr) {
        std::vector<std::string> func_names;
        std::string source = parse_expression(expr, func_names);
        
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

    bool XFunc::code_complete(const std::string& token, std::vector<std::string>& matches) {
        return false;
    }

    std::string XFunc::code_inspect(const std::string& word) {
        return "";
    }
}