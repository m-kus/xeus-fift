#pragma once

#include <ton/crypto/fift/Fift.h>
#include <ton/crypto/fift/words.h>

#include "helpers.hpp"

using namespace fift;
using namespace vm;
using namespace str;

namespace xfift {

    struct XResult
    {
        int code;
        std::string vmlog;
        std::string output;
        std::string ename;
        std::string evalue;
        std::vector<std::string> traceback;

        XResult();
        XResult(fift::IntError e);
        XResult(fift::Quit e);
    };

    class XFift
    {    
    private:

        std::vector<std::string> fift_path_;
        vm::TonDb ton_db_;
        fift::Dictionary dictionary_;
        fift::SourceLookup source_lookup_;
        fift::IntCtx ctx_;

    public:

        XFift();
        virtual ~XFift();

        void configure();
        XResult do_interpret(const std::string& expr);
        bool code_complete(const std::string& token, std::vector<std::string>& matches);
        std::string code_inspect(const std::string& word);      
    };
}