#include <td/utils/port/path.h>
#include <td/utils/PathView.h>
#include <td/utils/Slice.h>

#include "xfift.hpp"

namespace xfift {

    XResult::XResult()
        : code(0) 
    {
    }

    XResult::XResult(fift::IntError e)
        : code(-1)
        , ename("Interpreter error")
    {            
        std::vector<std::string> msg;
        str::split(e.msg, ':', msg);
        if (msg.size() >= 5) {  // text : filename : lineno : word : message [: due_error]
            std::stringstream tb;
            tb << "Word `" << str::strip(msg[3]) << "` at line " << msg[2];
            if (msg[1].size() > 2) {  // really a file
                tb << " (" << str::strip(msg[1]) << ")";
            }
            evalue = str::strip(msg[4]);
            traceback.push_back(tb.str());
            if (msg.size() == 6) {
                traceback.push_back("Due to underlying error: " + msg[5]);
            }
        } else {
            evalue = e.msg;
        }
    }

    XResult::XResult(fift::Quit e)
        : code(e.res)
        , ename("Abnormal termination")
        , evalue("code `" + std::to_string(e.res) + "`")
    {
    }

    XFift::XFift()
        : source_lookup_(std::make_unique<fift::OsFileLoader>())
    {
        ctx_.ton_db = &ton_db_;
        ctx_.dictionary = &dictionary_;
        ctx_.source_lookup = &source_lookup_;
        ctx_.include_depth = 1;
        ctx_.filename = 'stdin';
    }

    XFift::~XFift() 
    {
        if (ton_db_) {
            ton_db_.reset();
        }
    }

    void XFift::configure()
    {
        std::vector<std::string> source_include_path;
        const char* path = std::getenv("FIFTPATH");
        str::split(path ? path : "/usr/lib/fift", ':', source_include_path);

        auto r_current_dir = td::realpath(".");
        if (r_current_dir.is_ok()) {
            current_dir_ = r_current_dir.move_as_ok();
            ctx_.currentd_dir = current_dir_;
            source_include_path.push_back(current_dir_);
        }

        std::clog << "Include directories:\n";
        for (auto& path : source_include_path) {
            std::clog << path << std::endl;
            source_lookup_.add_include_path(path);
        }

        fift::init_words_common(dictionary_);
        fift::init_words_vm(dictionary_);
        fift::init_words_ton(dictionary_);
        std::clog << "Configuration successful\n";
    }

    XResult XFift::do_interpret(const std::string& expr) 
    {
        XResult res;
        std::stringstream ssin(expr), ssout, sserr;
        ctx_.input_stream = &ssin;
        ctx_.output_stream = &ssout;
        ctx_.error_stream = &sserr;        

        try {
            res.code = fift::funny_interpret_loop(ctx_);
            res.output = ssout.str();
            str::rstrip(res.output);
        } catch (fift::IntError ab) {
            res = XResult(ab);
            res.traceback.push_back(sserr.str());
        } catch (fift::Quit q) {
            res = XResult(q);
            res.traceback.push_back(sserr.str());
        }
        
        return std::move(res);
    }
}
