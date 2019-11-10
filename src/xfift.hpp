#pragma once

#include <iostream>
#include <sstream>
#include <streambuf>

#include <ton/crypto/fift/Fift.h>
#include <ton/crypto/fift/words.h>

#include "str_helpers.hpp"

using namespace fift;
using namespace vm;
using namespace str;

namespace xfift {

    struct XResult
    {
        int code;
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

        std::string current_dir_;
        vm::TonDb ton_db_;
        fift::Dictionary dictionary_;
        fift::SourceLookup source_lookup_;
        fift::IntCtx ctx_;

    public:

        XFift();
        virtual ~XFift();
        
        void configure();
        XResult do_interpret(const std::string& expr);
    };

}