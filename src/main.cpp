#include "xinterpreter.hpp"

#include <memory>
#include <getopt.h>

#include <xeus/xkernel.hpp>
#include <xeus/xkernel_configuration.hpp>
#include <td/utils/logging.h>
#include <td/utils/port/path.h>
#include <td/utils/Parser.h>


std::string extract_filename(int& argc, char* argv[])
{
    std::string res = "";
    for (int i = 0; i < argc; ++i)
    {
        if ((std::string(argv[i]) == "-f") && (i + 1 < argc))
        {
            res = argv[i + 1];
            for(int j = i; j < argc - 2; ++j)
            {
                argv[j]  = argv[j + 2];
            }
            argc -= 2;
            break;
        }
    }
    return res;
}

int main(int argc, char* argv[]) {
    std::string file_name = extract_filename(argc, argv);

    using interpreter_ptr = std::unique_ptr<xfift::interpreter>;
    interpreter_ptr interpreter = interpreter_ptr(new xfift::interpreter());

    if (!file_name.empty()) {
        xeus::xconfiguration config = xeus::load_configuration(file_name);
        xeus::xkernel kernel(config, xeus::get_user_name(), std::move(interpreter));
        
        std::clog <<
            "Starting xeus-fift kernel...\n\n"
            "If you want to connect to this kernel from an other client, you can use"
            " the " + file_name + " file.\n";

        kernel.start();
    } else {
        xeus::xkernel kernel(xeus::get_user_name(), std::move(interpreter));

        const auto& config = kernel.get_config();
        std::clog <<
            "Starting xeus-fift kernel...\n\n"
            "If you want to connect to this kernel from an other client, just copy"
            " and paste the following content inside of a `kernel.json` file. And then run for example:\n\n"
            "# jupyter console --existing kernel.json\n\n"
            "kernel.json\n```\n{\n"
            "    \"transport\": \"" + config.m_transport + "\",\n"
            "    \"ip\": \"" + config.m_ip + "\",\n"
            "    \"control_port\": " + config.m_control_port + ",\n"
            "    \"shell_port\": " + config.m_shell_port + ",\n"
            "    \"stdin_port\": " + config.m_stdin_port + ",\n"
            "    \"iopub_port\": " + config.m_iopub_port + ",\n"
            "    \"hb_port\": " + config.m_hb_port + ",\n"
            "    \"signature_scheme\": \"" + config.m_signature_scheme + "\",\n"
            "    \"key\": \"" + config.m_key + "\"\n"
            "}\n```\n";

        kernel.start();
    }

    return 0;
}