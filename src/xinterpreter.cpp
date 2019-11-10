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
        if (res.code == 0) 
        {
            nl::json pub_data;
            pub_data["text/plain"] = res.output;
            publish_execution_result(execution_counter, std::move(pub_data), nl::json());

            kernel_res["status"] = "ok";
        }
        else 
        {
            std::vector<std::string> traceback = res.traceback;
            traceback.insert(traceback.begin(), res.ename + ": " + res.evalue);
            publish_execution_error(res.ename, res.evalue, traceback);

            kernel_res["status"] = "error";
            kernel_res["ename"] = res.ename;
            kernel_res["evalue"] = res.evalue;
            kernel_res["traceback"] = res.traceback;
        }
        
        return kernel_res;
    }

    nl::json interpreter::complete_request_impl(const std::string& code,
                                                int cursor_pos)
    {
        nl::json result;
        result["status"] = "ok";
        result["matches"] = {};
        result["cursor_start"] = cursor_pos;
        result["cursor_end"] = cursor_pos;
        return result;
    }

    nl::json interpreter::inspect_request_impl(const std::string& code,
                                               int cursor_pos,
                                               int detail_level)
    {
        nl::json result;

        if (code.compare("print") == 0)
        {
            result["found"] = true;
            result["text/plain"] = "Print objects to the text stream file, [...]";
        }
        else
        {
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
        kernel_res["language_info"]["name"] = "fift";
        kernel_res["language_info"]["version"] = "0.5";
        kernel_res["language_info"]["mimetype"] = "text/x-fift";
        kernel_res["language_info"]["file_extension"] = ".fif";
        return kernel_res;
    }

    void interpreter::shutdown_request_impl() {
        std::clog << "Adios\n";
    }
}
