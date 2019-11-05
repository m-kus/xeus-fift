#include <iostream>

#include <ton/crypto/fift/words.h>

#include "fift_interpreter.hpp"

namespace xfift
{
    Fift build_interpreter(const Params& params) {
        fift::Fift::Config config;

        config.source_lookup = fift::SourceLookup(std::make_unique<fift::OsFileLoader>());
        for (auto& path : params.source_include_path) {
            config.source_lookup.add_include_path(path);
        }

        if (!params.ton_db_path.empty()) {
            auto r_ton_db = vm::TonDbImpl::open(params.ton_db_path);
            if (r_ton_db.is_error()) {
                LOG(ERROR) << "Error opening ton database: " << r_ton_db.error().to_string();
                std::exit(2);
            }
            config.ton_db = r_ton_db.move_as_ok();
            // TODO: reset at the end
        }

        fift::init_words_common(config.dictionary);
        fift::init_words_vm(config.dictionary);
        fift::init_words_ton(config.dictionary);

        fift::Fift fift(std::move(config));

        if (params.fift_preload) {
            auto status = fift.interpret_file("Fift.fif", "");
            if (status.is_error()) {
                LOG(ERROR) << "Error interpreting standard preamble file `Fift.fif`: " << status.error().message()
                           << "\nCheck that correct include path is set by -I or by FIFTPATH environment variable, "
                              "or disable standard preamble by -n.\n";
                std::exit(2);
            }
        }

        for (auto source : params.library_source_files) {
            auto status = fift.interpret_file(source, "");
            if (status.is_error()) {
                LOG(ERROR) << "Error interpreting preloaded file `" << source << "`: " << status.error().message();
                std::exit(2);
            }
        }

        return std::move(fift);
    }

    interpreter::interpreter(const Params& params)
        : fift_(build_interpreter(params))
    { 
    }

    interpreter::~interpreter() {}

    nl::json interpreter::execute_request_impl(int execution_counter, // Typically the cell number
                                               const std::string& /*code*/, // Code to execute
                                               bool /*silent*/,
                                               bool /*store_history*/,
                                               nl::json /*user_expressions*/,
                                               bool /*allow_stdin*/)
    {
        // You can use the C-API of your target language for executing the code,
        // e.g. `PyRun_String` for the Python C-API
        //      `luaL_dostring` for the Lua C-API

        // Use this method for publishing the execution result to the client,
        // this method takes the ``execution_counter`` as first argument,
        // the data to publish (mime type data) as second argument and metadata
        // as third argument.
        // Replace "Hello World !!" by what you want to be displayed under the execution cell
        nl::json pub_data;
        pub_data["text/plain"] = "Hello World !!";
        publish_execution_result(execution_counter, std::move(pub_data), nl::json());

        // You can also use this method for publishing errors to the client, if the code
        // failed to execute
        // publish_execution_error(error_name, error_value, error_traceback);
        publish_execution_error("TypeError", "123", {"!@#$", "*(*"});

        nl::json result;
        result["status"] = "ok";
        return result;
    }

    void interpreter::configure_impl()
    {
        // Perform some operations
    }

    nl::json interpreter::complete_request_impl(const std::string& code,
                                                       int cursor_pos)
    {
        nl::json result;

        // Code starts with 'H', it could be the following completion
        if (code[0] == 'H')
        {
            result["status"] = "ok";
            result["matches"] = {"Hello", "Hey", "Howdy"};
            result["cursor_start"] = 5;
            result["cursor_end"] = cursor_pos;
        }
        // No completion result
        else
        {
            result["status"] = "ok";
            result["matches"] = {};
            result["cursor_start"] = cursor_pos;
            result["cursor_end"] = cursor_pos;
        }

        return result;
    }

    nl::json interpreter::inspect_request_impl(const std::string& code,
                                                      int /*cursor_pos*/,
                                                      int /*detail_level*/)
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
        nl::json result;

        // if (is_complete(code))
        // {
            result["status"] = "complete";
        // }
        // else
        // {
        //    result["status"] = "incomplete";
        //    result["indent"] = 4;
        //}

        return result;
    }

    nl::json interpreter::kernel_info_request_impl()
    {
        nl::json result;
        result["implementation"] = "xeus-fift";
        result["implementation_version"] = "0.1.0";
        result["language_info"]["name"] = "fift";
        result["language_info"]["version"] = "0.5";
        result["language_info"]["mimetype"] = "text/x-fift";
        result["language_info"]["file_extension"] = ".fif";
        return result;
    }

    void interpreter::shutdown_request_impl() {
        std::cout << "Bye!!" << std::endl;
    }

}