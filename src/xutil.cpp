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

    std::string XToken::str() const {
        return line.substr(begin_pos, end_pos - begin_pos);
    }

    char XToken::prev_char() const {
        return begin_pos > 0 ? line[begin_pos - 1] : 0;
    }

    char XToken::next_char() const {
        return end_pos < line.size() ? line[end_pos] : 0;
    }

    XToken parse_token(
        const std::string& line, 
        std::size_t cursor_pos,
        const char* look_behind_chars,
        const char* look_ahead_chars) 
    {
        XToken token{};
        token.line = line;

        if (cursor_pos == 0) {
            token.begin_pos = 0;
        } else {
            token.begin_pos = line.find_last_of(look_behind_chars, cursor_pos > 0 ? cursor_pos - 1 : 0);
            if (token.begin_pos == std::string::npos) {
                token.begin_pos = 0;
            } else {
                token.begin_pos++;
            }
        }
        token.end_pos = line.find_first_of(look_ahead_chars, cursor_pos); 
        if (token.end_pos == std::string::npos) {
            token.end_pos = line.size();
        }
        return std::move(token);
    }

    XToken get_last_expression(const std::string& s, char ending)
    {
        XToken token{};
        token.line = s;

        token.end_pos = s.find_last_not_of(" \t\r\n");
        if (token.end_pos == std::string::npos) {
            token.end_pos = s.size();
        } else {
            token.end_pos++;
        }

        token.begin_pos = s.find_last_of('\n', token.end_pos);
        if (token.begin_pos == std::string::npos) {
            token.begin_pos = 0;
        } else {
            token.begin_pos++;
        }

        auto ending_pos = s.find_first_of(ending, token.begin_pos);
        if (ending_pos != std::string::npos) {
            token.end_pos = ending_pos;
        }
        
        return std::move(token);
    }
}