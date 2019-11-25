#include "xfunc.hpp"
#include "xfift/xfift.hpp"

using namespace xfift;
using namespace funC;
using namespace src;

namespace xfunc {

    void XFunc::configure() {
        funC::define_keywords();
        funC::define_builtins();
        funC::verbosity = 5;
    }

    inline std::string format_main(const std::string& expr) {
        std::stringstream ss;
        ss << "_ main() {\n" << expr;
        std::size_t ending = expr.find_last_not_of(" \r\n\t");
        if (ending != -1 && expr[ending] != ';') {
            ss << ';';
        }
        ss << "\n}";
        return ss.str();
    }

    XResult XFunc::do_parse(const std::string& expr) {
        global_sym_guard sym_guard(expr);
        std::stringstream ss(sym_guard.not_a_func() ? format_main(expr) : expr);
        src::FileDescr fd{"stdin", true};

        try {
            funC::parse_source(&ss, &fd);
        } catch (src::Fatal& fatal) {
            return XResult(fatal);
        } catch (src::ParseError& error) {
            return XResult(error);
        } catch (funC::UnifyError& unif_err) {
            return XResult(unif_err);
        }

        return XResult();
    }

    XResult XFunc::do_interpret() {
        XFift fift;
        fift.configure();

        try {
            std::string script = generate_fift_script();
            XResult res = fift.do_interpret(script);
            if (res.code == 0) {
                return res.vm_result();
            } else {
                return res;
            }
        } catch (src::Fatal& fatal) {
            return XResult(fatal);
        } catch (src::ParseError& error) {
            return XResult(error);
        }

        return XResult();
    }

    bool XFunc::code_complete(const std::string& token, std::vector<std::string>& matches) {
        return false;
    }
}