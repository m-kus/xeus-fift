#include "xutil.hpp"

namespace xfift {

    XResult::XResult()
        : code(0) 
    {
    }

    XResult::XResult(const std::string& msg)
        : code(-1)
        , ename("Xeus-fift error")
        , evalue(msg)
    {
        traceback.push_back(ename + ": " + evalue);
    }

    XResult::XResult(fift::IntError e)
        : code(-1)
        , ename("Interpreter error")
    {            
        std::vector<std::string> msg;
        split(e.msg, ':', msg);
        if (msg.size() >= 5) {  // text : filename : lineno : word : message [: due_error]
            std::stringstream tb;
            tb << "Word `" << strip(msg[3]) << "` at line " << msg[2];
            if (msg[1].size() > 2) {  // really a file
                tb << " (" << strip(msg[1]) << ")";
            }
            traceback.push_back(tb.str());
            if (msg.size() == 6) {
                traceback.push_back("Due to underlying error: " + msg[5]);
            }
            evalue = strip(msg[4]);
        } else {
            evalue = e.msg;
        }

        traceback.insert(traceback.begin(), ename + ": " + evalue);
    }

    XResult::XResult(fift::Quit e)
        : code(e.res)
        , ename("Abnormal termination")
        , evalue("code `" + std::to_string(e.res) + "`")
    {
        traceback.push_back(ename + ": " + evalue);
    }

    XResult::XResult(src::Fatal& fatal)
        : code(-1)
        , ename("Fatal error")
        , evalue(fatal.get_msg())
    {
        traceback.push_back(ename + ": " + evalue);
    }

    XResult::XResult(src::ParseError& error)
        : code(-1)
        , ename("Parser error")
        , evalue(error.message)
    {
        traceback.push_back(ename + ": " + evalue);

        std::stringstream tb;
        if (error.where.show_context(tb)) {
            traceback.push_back(tb.str());
        }
    }

    XResult::XResult(funC::UnifyError& unif_err)
        : code(-1)
        , ename("Unify error")
    {
        std::stringstream tb;
        tb << "Cannot unify type " << unif_err.te1 << " with " << unif_err.te2;

        if (unif_err.msg.empty()) {
            evalue = tb.str();
        } else {
            evalue = unif_err.msg;
            traceback.push_back(tb.str());
        }

        traceback.insert(traceback.begin(), ename + ": " + evalue);
    }
}