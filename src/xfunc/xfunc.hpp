#pragma once

#include <regex>
#include <unordered_map>
#include <ton/crypto/func/func.h>

#include "xutil.hpp"

using namespace sym;

namespace xfift {

    void reset_global_state();
    void resolve_includes(std::string& expr);
    void parse_expression(std::string& expr, std::vector<std::string>& func_names);
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
    private:

        std::vector<std::string> func_path_;

    public:

        XFunc() {}
        virtual ~XFunc();

        void configure();
        XResult do_interpret(const std::string& expr);
        XToken parse_token(const std::string& line, std::size_t cursor_pos);
        bool code_complete(const XToken& token, std::vector<std::string>& matches);
        std::string code_inspect(const XToken& token);
    };
}