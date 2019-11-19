#pragma once

#define XEUS_FIFT_VERSION_MAJOR 0
#define XEUS_FIFT_VERSION_MINOR 1
#define XEUS_FIFT_VERSION_PATCH 0

#define XEUS_FIFT_CONCATENATE(A, B) XEUS_FIFT_CONCATENATE_IMPL(A, B)
#define XEUS_FIFT_CONCATENATE_IMPL(A, B) A##B
#define XEUS_FIFT_STRINGIFY(a) XEUS_FIFT_STRINGIFY_IMPL(a)
#define XEUS_FIFT_STRINGIFY_IMPL(a) #a

#define XEUS_FIFT_VERSION XEUS_FIFT_STRINGIFY(XEUS_FIFT_CONCATENATE(XEUS_FIFT_VERSION_MAJOR,   \
                        XEUS_FIFT_CONCATENATE(.,XEUS_FIFT_CONCATENATE(XEUS_FIFT_VERSION_MINOR,   \
                                                XEUS_FIFT_CONCATENATE(.,XEUS_FIFT_VERSION_PATCH)))))

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