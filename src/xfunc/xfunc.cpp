#include "xfunc.hpp"
#include "xfift/xfift.hpp"

using namespace xfift;
using namespace funC;
using namespace src;

namespace xfift {

    std::string parse_expression(const std::string& expr, std::vector<std::string>& func_names) {
        std::regex func_name_re("\\s~?([_\\w]+)\??\\([_,\\w\\s]*\\)\\s?(?:[a-z]|\\{)");
       
        for (std::sregex_iterator it = std::sregex_iterator(expr.begin(), expr.end(), func_name_re);
                                  it != std::sregex_iterator();
                                ++it)
        {
            std::smatch match = *it;
            assert(match.size() == 2);
            func_names.push_back(match.str(1));
        }

        if (func_names.empty()) {
            func_names.push_back("main");

            std::stringstream ss;
            ss << "_ main() {\n" << expr;
            std::size_t ending = expr.find_last_not_of(" \r\n\t");
            if (ending != -1 && expr[ending] != ';') {
                ss << ';';
            }
            ss << "\n}";
            return ss.str();
        } else {
            return expr;
        }
    }

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