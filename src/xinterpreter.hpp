#pragma once

#include <experimental/filesystem>
#include <xeus/xinterpreter.hpp>
#include <nlohmann/json.hpp>

#include "xfift/xfift.hpp"
#include "xfunc/xfunc.hpp"
#include "version.h"

namespace nl = nlohmann;
using namespace xeus;

namespace xfift {

    inline nl::json make_pub_data(XResult res) {
        std::stringstream ss;
        if (!res.vmlog.empty()) {
            ss << "<pre style=\"background-color: #ffe7d1; padding: 10px;\">" << html_escape(res.vmlog) << "</pre>";
        }
        ss << "<pre>" << html_escape(res.output) << "</pre>";

        nl::json data;
        data["text/html"] = ss.str();
        data["text/plain"] = res.output;
        return std::move(data);
    }

    template<typename XInt>
    class interpreter : public xeus::xinterpreter
    {
    private:

        XInt int_;

        void configure_impl() override;
        nl::json execute_request_impl(int, const std::string&, bool, bool, nl::json, bool) override;
        nl::json complete_request_impl(const std::string&, int) override;
        nl::json inspect_request_impl(const std::string&, int, int) override;
        nl::json is_complete_request_impl(const std::string&) override;
        nl::json kernel_info_request_impl() override;
        void shutdown_request_impl() override;

    };

    template<typename XInt>
    void interpreter<XInt>::configure_impl()
    {
        int_.configure();
    }

    template<typename XInt>
    nl::json interpreter<XInt>::execute_request_impl(
        int execution_counter,
        const std::string& code,
        bool silent,
        bool /*store_history*/,
        nl::json /*user_expressions*/,
        bool /* allow_stdin */)
    {
        nl::json kernel_res;

        auto res = int_.do_interpret(code);
        if (res.code == 0) {
            kernel_res["status"] = "ok";
            if (!silent) {
                publish_execution_result(execution_counter, make_pub_data(res), nl::json::object());
            }
        } else {
            kernel_res["status"] = "error";
            kernel_res["ename"] = res.ename;
            kernel_res["evalue"] = res.evalue;
            kernel_res["traceback"] = res.traceback;
            publish_execution_error(res.ename, res.evalue, res.traceback);
        }
        
        return kernel_res;
    }

    template<typename XInt>
    nl::json interpreter<XInt>::complete_request_impl(
        const std::string& code, 
        int cursor_pos)
    {
        nl::json kernel_res;

        std::vector<std::string> matches;
        XToken token = int_.code_complete(code, cursor_pos, matches);

        if (!matches.empty()) {
            kernel_res["matches"] = matches;
            kernel_res["cursor_start"] = token.begin_pos;
            kernel_res["cursor_end"] = token.end_pos;
        } else {
            kernel_res["matches"] = nl::json::array();
            kernel_res["cursor_start"] = cursor_pos;
            kernel_res["cursor_end"] = cursor_pos;
        }

        kernel_res["status"] = "ok";
        return kernel_res;
    }

    template<typename XInt>
    nl::json interpreter<XInt>::inspect_request_impl(
        const std::string& code,
        int cursor_pos,
        int /* detail_level */  )
    {
        nl::json kernel_res;
  
        std::string tooltip;
        int_.code_inspect(code, cursor_pos, tooltip);

        if (!tooltip.empty()) {
            kernel_res["found"] = true;
            kernel_res["data"]["text/plain"] = tooltip;
        } else {
            kernel_res["found"] = false;
        }

        kernel_res["status"] = "ok";
        return kernel_res;
    }

    template<typename XInt>
    nl::json interpreter<XInt>::is_complete_request_impl(const std::string& /*code*/)
    {
        nl::json kernel_res;
        kernel_res["status"] = "complete";
        kernel_res["indent"] = "";
        return kernel_res;
    }

    template<typename XInt>
    nl::json interpreter<XInt>::kernel_info_request_impl()
    {
        return nl::json::object();
    }

    template<typename XInt>
    void interpreter<XInt>::shutdown_request_impl()
    {
    }

    template<>
    nl::json interpreter<XFift>::kernel_info_request_impl()
    {
        nl::json kernel_res;
        kernel_res["implementation"] = "xeus-fift";
        kernel_res["implementation_version"] = XEUS_FIFT_VERSION;
        kernel_res["language_info"]["name"] = "fift";
        kernel_res["language_info"]["version"] = "0.5";
        kernel_res["language_info"]["mimetype"] = "text/x-fift";
        kernel_res["language_info"]["codemirror_mode"] = "fift";
        kernel_res["language_info"]["file_extension"] = ".fif";
        return kernel_res;
    }

    template<>
    nl::json interpreter<XFunc>::kernel_info_request_impl()
    {
        nl::json kernel_res;
        kernel_res["implementation"] = "xeus-func";
        kernel_res["implementation_version"] = XEUS_FIFT_VERSION;
        kernel_res["language_info"]["name"] = "func";
        kernel_res["language_info"]["version"] = "0.5";
        kernel_res["language_info"]["mimetype"] = "text/x-func";
        kernel_res["language_info"]["codemirror_mode"] = "func";
        kernel_res["language_info"]["file_extension"] = ".fc";
        return kernel_res;
    }
}