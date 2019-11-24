#include <td/utils/port/path.h>
#include <td/utils/PathView.h>
#include <td/utils/Slice.h>

#include "xfift.hpp"
#include "docstring.hpp"

namespace xfift {

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
        SET_VERBOSITY_LEVEL(verbosity_DEBUG);

        const char* path = std::getenv("FIFTPATH");
        split(path ? path : "/usr/lib/fift", ':', fift_path_);

        auto r_current_dir = td::realpath(".");
        if (r_current_dir.is_ok()) {
            ctx_.currentd_dir = r_current_dir.move_as_ok();
            fift_path_.push_back(ctx_.currentd_dir);
        }

        std::clog << "Include directories:\n";
        for (auto& path : fift_path_) {
            std::clog << path << std::endl;
            source_lookup_.add_include_path(path);
        }

        fift::init_words_common(dictionary_);
        fift::init_words_vm(dictionary_);
        fift::init_words_ton(dictionary_);

        do_interpret("\"Fift.fif\" include");
        do_interpret("\"Asm.fif\" include");

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
        } catch (fift::IntError ab) {
            res = XResult(ab);
        } catch (fift::Quit q) {
            res = XResult(q);
        }

        res.output = strip(ssout.str());
        res.vmlog = sserr.str();        
        return std::move(res);
    }

    bool XFift::code_complete(const std::string& token, std::vector<std::string>& matches)
    {
        if (!token.empty() && token[0] == '"') {
            fs::path p{token.substr(1)};
            if (p.has_parent_path()) {
                path_complete(p, matches);
            } else {
                for (auto& dir : fift_path_) {
                    path_complete(fs::path{dir} / p, matches);
                }
            }
        } else {
            std::for_each(dictionary_.begin(), dictionary_.end(), [&](const std::pair<std::string, WordRef>& x) {
                if (token.empty() || std::equal(token.begin(), token.end(), x.first.begin())) {
                    matches.push_back(strip(x.first));
                }
            });
        }

        return !matches.empty(); 
    }

    std::string XFift::code_inspect(const std::string& word) 
    {
        return get_docstring(word);
    }
}
