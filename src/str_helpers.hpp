#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

#include <td/utils/Parser.h>

namespace str {

    static inline std::string &lstrip(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
    }

    static inline std::string &rstrip(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(),
                std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
    }

    static inline std::string &strip(std::string &s) {
        return lstrip(rstrip(s));
    }

    static inline void split(std::string s, char delim, std::vector<std::string>& res) {
        td::Parser parser(s);
        while (!parser.empty()) {
            auto chunk = parser.read_till_nofail(delim);
            if (!chunk.empty()) {
                res.push_back(chunk.str());
            }
            parser.skip_nofail(delim);
        }
    }
}