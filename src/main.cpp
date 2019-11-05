#include <memory>
#include <getopt.h>

#include <xeus/xkernel.hpp>
#include <xeus/xkernel_configuration.hpp>
#include <td/utils/logging.h>
#include <td/utils/port/path.h>
#include <td/utils/Parser.h>

#include "fift_interpreter.hpp"

void usage(const char* progname) {
  std::cerr << "Jupyter kernel for the Fift smart contract language\n";
  std::cerr
      << "usage: " << progname
      << " [-n] [-I <source-include-path>] {-L <library-fif-file>} [-f <kernel-json-file>]\n";
  std::cerr << "\t-n\t\t\tDo not preload standard preamble file `Fift.fif`\n"
               "\t-I<source-search-path>\tSets colon-separated library source include path. If not indicated, "
               "$FIFTPATH is used instead.\n"
               "\t-L<library-fif-file>\tPre-loads a library source file\n"
               "\t-d<ton-db-path>\t\tUse a ton database\n"
               "\t-v<verbosity-level>\tSet verbosity level\n"
               "\t-f<kernel-json-file>\t\n";
  std::exit(2);
}

void parse_include_path_set(std::string include_path_set, std::vector<std::string>& res) {
    td::Parser parser(include_path_set);
    while (!parser.empty()) {
        auto path = parser.read_till_nofail(':');
        if (!path.empty()) {
            res.push_back(path.str());
        }
        parser.skip_nofail(':');
    }
}

xfift::Params load_params(int argc, char* argv[]) {
    xfift::Params params;
    int i = 0;

    while ((i = getopt(argc, argv, "hnI:L:d:v:f:")) != -1) {
        switch (i) {
            case 'n':
                params.fift_preload = false;
                break;
            case 'I':
                parse_include_path_set(optarg, params.source_include_path);
                params.no_env = true;
                break;
            case 'L':
                params.library_source_files.emplace_back(optarg);
                break;
            case 'd':
                params.ton_db_path = optarg;
                break;
            case 'v':
                params.verbosity_level = VERBOSITY_NAME(FATAL) + td::to_integer<int>(td::Slice(optarg));
                break;
            case 'f':
                params.kernel_settings_path = optarg;
            case 'h':
            default:
                usage(argv[0]);
        }
    }

    if (optind < argc) {
        usage(argv[0]);
    }

    if (!params.no_env) {
        const char* path = std::getenv("FIFTPATH");
        parse_include_path_set(path ? path : "/usr/lib/fift", params.source_include_path);
    }

    auto r_current_dir = td::realpath(".");
    if (r_current_dir.is_ok()) {
        std::string current_dir = r_current_dir.move_as_ok();
        params.source_include_path.push_back(current_dir);
    }

    return std::move(params);
}

int main(int argc, char* argv[]) {
    xfift::Params params = load_params(argc, argv);
    SET_VERBOSITY_LEVEL(params.verbosity_level);

    using interpreter_ptr = std::unique_ptr<xfift::interpreter>;
    interpreter_ptr interpreter = interpreter_ptr(new xfift::interpreter(params));

    if (!params.kernel_settings_path.empty()) {
        xeus::xconfiguration config = xeus::load_configuration(params.kernel_settings_path);
        xeus::xkernel kernel(config, xeus::get_user_name(), std::move(interpreter));
        
        std::clog <<
            "Starting xeus-fift kernel...\n\n"
            "If you want to connect to this kernel from an other client, you can use"
            " the " + params.kernel_settings_path + " file."
            << std::endl;

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