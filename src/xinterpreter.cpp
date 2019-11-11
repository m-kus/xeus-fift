#include "xinterpreter.hpp"

namespace xfift
{
    void interpreter::configure_impl()
    {
        fift_.configure();
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

            nl::json pub_data;
            pub_data["text/plain"] = res.output;
            publish_execution_result(execution_counter, std::move(pub_data), nl::json());
        } else {
            kernel_res["status"] = "error";
            kernel_res["ename"] = res.ename;
            kernel_res["evalue"] = res.evalue;
            kernel_res["traceback"] = res.traceback;

            std::vector<std::string> traceback = res.traceback;
            traceback.insert(traceback.begin(), res.ename + ": " + res.evalue);
            publish_execution_error(res.ename, res.evalue, traceback);
        }
        
        return kernel_res;
    }

    nl::json interpreter::complete_request_impl(const std::string& code,
                                                int cursor_pos)
    {
        nl::json kernel_res;
        std::vector<std::string> matches;
        std::size_t word_offset = fift_.code_complete(code, cursor_pos, matches);

        if (!matches.empty()) {
            kernel_res["matches"] = matches;
            kernel_res["cursor_start"] = word_offset;
        } else {
            kernel_res["matches"] = {};
            kernel_res["cursor_start"] = cursor_pos;
        }

        kernel_res["status"] = "ok";
        kernel_res["cursor_end"] = cursor_pos;
        return kernel_res;
    }

    nl::json interpreter::inspect_request_impl(const std::string& code,
                                               int cursor_pos,
                                               int detail_level)
    {
        nl::json result;
        std::string docstring = fift_.code_inspect(code, cursor_pos);
       
        if (!docstring.empty()) {
            result["found"] = true;
            result["text/plain"] = docstring;
        } else {
            result["found"] = false;
        }

        result["status"] = "ok";
        return result;
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
        kernel_res["implementation_version"] = "0.1.0";
        kernel_res["language_info"]["name"] = "forth";
        kernel_res["language_info"]["version"] = "0.5";
        kernel_res["language_info"]["mimetype"] = "text/x-forth";
        kernel_res["language_info"]["file_extension"] = ".fif";
        return kernel_res;
    }

    void interpreter::shutdown_request_impl()
    {
    }
}
