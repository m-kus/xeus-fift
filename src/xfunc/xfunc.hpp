#pragma once

#include <regex>
#include <unordered_map>
#include <ton/crypto/func/func.h>

#include "xutil.hpp"

using namespace sym;

namespace xfift {

    void reset_global_state();
    std::string generate_fift_script();

    class global_sym_guard
    {
    private:

        std::unordered_map<sym::sym_idx_t, sym::SymDef*> backup_;

    public:

        global_sym_guard() = delete;
        global_sym_guard(const std::vector<std::string>& func_names);
        virtual ~global_sym_guard();
    };

    class XFunc
    {
    public:

        XFunc() {}
        virtual ~XFunc();

        void configure();
        XResult do_interpret(const std::string& expr);
        bool code_complete(const std::string& token, std::vector<std::string>& matches);
        std::string code_inspect(const std::string& word);
    };
}