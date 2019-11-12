#pragma once

#include <td/utils/Parser.h>

namespace str {

    static inline std::string strip(const std::string& s) 
    {
        if (s.empty()) {
            return s;
        }
        std::size_t first_scan = s.find_first_not_of(' ');
        std::size_t first = first_scan == std::string::npos ? s.length() : first_scan;
        std::size_t last = s.find_last_not_of(' ');
        return std::move(s.substr(first, last - first + 1));
    }

    static inline void split(std::string s, char delim, std::vector<std::string>& res) 
    {
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

namespace path {

    static inline std::size_t complete(const std::string& prefix,
                                       std::vector<std::string>& matches, 
                                       const std::string& extension = "") {
        return 0;
    }
}