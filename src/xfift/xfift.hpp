#pragma once

#include <ton/crypto/fift/Fift.h>
#include <ton/crypto/fift/words.h>

#include "xutil.hpp"

using namespace fift;
using namespace vm;

namespace xfift {

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
        XToken code_complete(const std::string& expr, std::size_t cursor_pos, std::vector<std::string>& matches);
        XToken code_inspect(const std::string& expr, std::size_t cursor_pos, std::string& tooltip);
    };
}