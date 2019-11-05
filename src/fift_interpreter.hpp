#pragma once

#include <xeus/xinterpreter.hpp>
#include <nlohmann/json.hpp>
#include <ton/crypto/fift/Fift.h>

using xeus::xinterpreter;
using fift::Fift;

namespace nl = nlohmann;

namespace xfift
{
    struct Params {
        std::vector<std::string> library_source_files;
        std::vector<std::string> source_include_path;
        std::string ton_db_path;
        std::string kernel_settings_path;
        bool fift_preload = true;
        bool no_env = false;
        int verbosity_level = VERBOSITY_NAME(INFO);
    };

    class interpreter : public xinterpreter
    {

    public:

        interpreter(const Params&);
        virtual ~interpreter();

    private:

        void configure_impl() override;

        nl::json execute_request_impl(
            int execution_counter,
            const std::string& code,
            bool silent,
            bool store_history,
            nl::json user_expressions,
            bool allow_stdin
        ) override;

        nl::json complete_request_impl(
            const std::string& code,
            int cursor_pos
        ) override;

        nl::json inspect_request_impl(
            const std::string& code,
            int cursor_pos,
            int detail_level
        ) override;

        nl::json is_complete_request_impl(const std::string& code) override;

        nl::json kernel_info_request_impl() override;

        void shutdown_request_impl() override;

    private:

        Fift fift_;
    };
}
