#pragma once

#include <regex>
#include <unordered_set>
#include <unordered_map>
#include <ton/crypto/func/func.h>

#include "xutil.hpp"

using namespace sym;

namespace xfift {

    void reset_global_state();
    void resolve_includes(std::string& expr, const std::vector<std::string>& func_path);
    void parse_functions(const std::string& expr, std::vector<std::string>& func_names);
    void force_main(std::string& expr, std::vector<std::string>& func_names);
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
        std::unordered_set<std::string> completion_;

    public:

        XFunc();
        virtual ~XFunc();

        void configure();
        XResult do_interpret(const std::string& expr);
        XToken code_complete(const std::string& expr, std::size_t cursor_pos, std::vector<std::string>& matches);
        XToken code_inspect(const std::string& expr, std::size_t cursor_pos, std::string& tooltip);
    };
}