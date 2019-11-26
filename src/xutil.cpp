#include <regex>

#include "xutil.hpp"

namespace xfift {

    std::string strip(const std::string& s) 
    {
        if (s.empty()) {
            return s;
        }
        std::size_t first_scan = s.find_first_not_of(' ');
        std::size_t first = first_scan == std::string::npos ? s.length() : first_scan;
        std::size_t last = s.find_last_not_of(' ');
        return std::move(s.substr(first, last - first + 1));
    }

    void split(std::string s, char delim, std::vector<std::string>& res) 
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

    token_pos parse_token(const std::string& line, std::size_t cursor_pos, std::string& res) 
    {
        auto token_begin = line.find_last_of(' ', cursor_pos > 0 ? cursor_pos - 1 : 0);
        if (token_begin == std::string::npos) {
            token_begin = 0;
        } else {
            token_begin++;
        }
        auto token_end = line.find_first_of(" \"", cursor_pos); 
        if (token_end == std::string::npos) {
            token_end = line.size();
        }
        res = line.substr(token_begin, token_end - token_begin);
        return std::make_pair(token_begin, token_end);
    }

    std::string html_escape(const std::string& data) 
    {
        std::string buffer;
        buffer.reserve(data.size());
        for(size_t pos = 0; pos != data.size(); ++pos) {
            switch(data[pos]) {
                case '&':  buffer.append("&amp;");       break;
                case '\"': buffer.append("&quot;");      break;
                case '\'': buffer.append("&apos;");      break;
                case '<':  buffer.append("&lt;");        break;
                case '>':  buffer.append("&gt;");        break;
                default:   buffer.append(&data[pos], 1); break;
            }
        }
        return std::move(buffer);
    }

    std::size_t path_complete(const fs::path& path, std::vector<std::string>& matches) 
    {
        fs::path directory, prefix;
        if (fs::is_directory(path)) {
            directory = path;
        } else if (path.has_parent_path()) {
            directory = path.parent_path();
            prefix = path.filename();
        } else {
            directory = fs::current_path();
            prefix = path;
        }

        if (fs::exists(directory)) {
            for(auto& file : fs::directory_iterator(directory)) {
                auto filename = file.path().filename().string();
                if (prefix.empty() || 0 == filename.find(prefix)) {
                    matches.push_back(file.path());
                }
            }
        }

        return matches.size();
    }

    XResult XResult::vm_result() {
        std::regex vm_output_re("^(.+)\\s([0-9]{1,2})\\s\\n$");
        std::smatch match;

        if (std::regex_match(output, match, vm_output_re)) {
            assert(match.size() == 3);
            
            XResult res;
            res.code = std::stoi(match.str(2));
            res.output = match.str(1);
            res.vmlog = vmlog;

            if (res.code != 0) {
                res.ename = "VM error";
                if (res.code < (int)vm::Excno::total) {
                    res.evalue = vm::get_exception_msg(static_cast<vm::Excno>(res.code));
                } else {
                    res.evalue = "unknown code " + match.str(2);
                }
                res.traceback.push_back(res.ename + ": " + res.evalue);
                for (auto& row : traceback) {
                    res.traceback.push_back(row);
                }
            }
            
            return res;
        } else {
            return *this;
        }
    }
}