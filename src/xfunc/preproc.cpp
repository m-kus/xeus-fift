#include <fstream>
#include <streambuf>
#include <iostream> 
using namespace std; 

#include "xfunc.hpp"

namespace xfift {

    const std::vector<std::string> skip_list = {
        "int", "cell", "slice", "builder", "cont", "tuple",
        "return", "var", "repeat", "do", "while", "until", "if", "ifnot", "then", "else", "elseif", "elseifnot",
        "extern", "asm", "impure", "inline", "inline_ref", "method_id", "operator", "infix", "infixl", "infixr",
        "divmod", "moddiv", "muldivr", "muldiv", "muldivmod", "null?", "throw", "throw_if", "throw_unless", 
        "load_int", "load_uint", "preload_int", "preload_uint", "store_int", "store_uint", "load_bits", 
        "preload_bits", "int_at", "cell_at", "slice_at", "tuple_at", "at", "touch", "touch2", "dump", 
        "run_method0", "run_method1", "run_method2", "run_method3", "~divmod", "~moddiv", "~store_int", 
        "~store_uint", "~touch", "~touch2"
    };

    void resolve_includes(std::string& expr, const std::vector<std::string>& func_path) {
        std::regex include_re(R"(#include\s\"([^\"]+)\")");
        std::smatch match;
        std::stringstream ss;
        bool resolved = false;

        while (std::regex_search(expr, match, include_re)) {
            assert(match.size() == 2);
            expr = match.suffix();

            fs::path path{match.str(1)};
            if (!path.has_parent_path()) {
                for (auto& dir : func_path) {
                    auto p = fs::path{dir} / path;
                    if (fs::exists(p)) {
                        path = p;
                        break;
                    }
                }
            }

            std::ifstream include(path);
            if (include.fail()) {
                throw "failed to load `" + path.string() + "`";
            }
            ss << include.rdbuf() << "\n";
            resolved = true;
        }

        if (resolved) {
            ss << expr;
            expr = ss.str();
        }
    }

    void parse_functions(const std::string& expr, std::vector<std::string>& func_names) {
        std::regex func_name_re(R"(\s(~?[_\w:#]+[?=]?) *\([_,\w\s]*\) *(?:[a-z]|\{))");
       
        for (std::sregex_iterator it = std::sregex_iterator(expr.begin(), expr.end(), func_name_re);
                                  it != std::sregex_iterator();
                                ++it)
        {   
            std::smatch match = *it;
            assert(match.size() == 2);
            if (std::find(skip_list.begin(), skip_list.end(), match.str(1)) == skip_list.end()) {
                func_names.push_back(match.str(1));
            }
        }

        std::clog << "Found " << func_names.size() << " function declarations\n";
    }

    void force_main(std::string& expr, std::vector<std::string>& func_names) {
        if (func_names.empty()) {
            func_names.push_back("main");

            std::stringstream ss;
            ss << "_ main() impure {\n" << expr;
            std::size_t ending = expr.find_last_not_of(" \r\n\t");
            if (ending != std::string::npos && expr[ending] != ';') {
                ss << ';';
            }
            ss << "\n}";
            expr = ss.str();
        }
    }
}