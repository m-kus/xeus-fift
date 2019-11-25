#pragma once

#include <regex>
#include <unordered_map>

#include <ton/crypto/func/func.h>

#include "xutil.hpp"

using namespace sym;
using namespace xutil;

namespace xfunc {

    std::string generate_fift_script();

    class global_sym_guard
    {
    private:

        std::unordered_map<sym::sym_idx_t, sym::SymDef*> backup_;
        std::vector<std::string> func_names_;

    public:

        global_sym_guard() = delete;
        global_sym_guard(const std::string& expr);
        virtual ~global_sym_guard();

        bool not_a_func() const;
    };

    class XFunc
    {
    public:

        XFunc() {}
        virtual ~XFunc() {}

        void configure();
        XResult do_parse(const std::string& expr);
        XResult do_interpret();
        bool code_complete(const std::string& token, std::vector<std::string>& matches);
    };
}