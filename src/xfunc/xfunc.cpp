#include <ton/crypto/func/func.h>

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
        ss << "void main() {\n" << expr << "\n}";
        return ss.str();
    }

    XResult XFunc::do_parse(const std::string& expr) {
        global_sym_guard sym_guard(expr);
        if (sym_guard.has("main")) {
            return XResult("Put main's body in the cell instead");
        }
        
        std::stringstream ss(sym_guard.empty() ? format_main(expr) : expr);
        src::FileDescr fd{"stdin", true};

        try {
            funC::parse_source(&ss, &fd);
            sym_guard.analyze_new();
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
            return fift.do_interpret(script);
        } catch (src::Fatal& fatal) {
            return XResult(fatal);
        } catch (src::ParseError& error) {
            return XResult(error);
        }
    }

    bool XFunc::code_complete(const std::string& token, std::vector<std::string>& matches) {
        return false;
    }
}