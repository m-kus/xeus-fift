#include "xinterpreter.hpp"

namespace xfift
{
    void interpreter::configure_impl()
    {
        fift_.configure();
    }

    nl::json make_pub_data(XResult res) {
        std::stringstream ss;
        if (!res.vmlog.empty()) {
            ss << "<pre style=\"background-color: #ffe7d1; padding: 10px;\">" << html_escape(res.vmlog) << "</pre>";
        }
        ss << "<pre>" << str::html_escape(res.output) << "</pre>";

        nl::json data;
        data["text/html"] = ss.str();
        return std::move(data);
    }

    nl::json interpreter::execute_request_impl(int execution_counter,
                                               const std::string& code,
                                               bool silent,
                                               bool /*store_history*/,
                                               nl::json /*user_expressions*/,
                                               bool allow_stdin)
    {
        nl::json kernel_res;

        auto res = fift_.do_interpret(code);
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

    nl::json interpreter::complete_request_impl(const std::string& code, int cursor_pos)
    {
        nl::json kernel_res;
        std::string token;
        std::vector<std::string> matches;

        auto token_pos = str::parse_token(code, cursor_pos, token);

        if (fift_.code_complete(token, matches)) {
            kernel_res["matches"] = matches;
            kernel_res["cursor_start"] = token[0] == '"' ? token_pos.first + 1 : token_pos.first;
            kernel_res["cursor_end"] = token_pos.second;
        } else {
            kernel_res["matches"] = nl::json::array();
            kernel_res["cursor_start"] = cursor_pos;
            kernel_res["cursor_end"] = cursor_pos;
        }

        kernel_res["status"] = "ok";
        return kernel_res;
    }

    nl::json interpreter::inspect_request_impl(const std::string& code,
                                               int cursor_pos,
                                               int detail_level)
    {
        nl::json kernel_res;
        std::string token;

        str::parse_token(code, cursor_pos, token);
        std::string docstring = fift_.code_inspect(token);

        if (!docstring.empty()) {
            kernel_res["found"] = true;
            kernel_res["data"]["text/plain"] = docstring;
        } else {
            kernel_res["found"] = false;
        }

        kernel_res["status"] = "ok";
        return kernel_res;
    }

    nl::json interpreter::is_complete_request_impl(const std::string& /*code*/)
    {
        nl::json kernel_res;
        kernel_res["status"] = "complete";
        kernel_res["indent"] = "";
        return kernel_res;
    }

    nl::json interpreter::kernel_info_request_impl()
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

    void interpreter::shutdown_request_impl()
    {
    }
}
