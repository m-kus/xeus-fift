#pragma once

#include <experimental/filesystem>
#include <td/utils/Parser.h>
#include <ton/crypto/fift/Fift.h>
#include <ton/crypto/fift/words.h>
#include <ton/crypto/func/func.h>
#include <ton/crypto/parser/symtable.h>

namespace fs = std::experimental::filesystem;
using namespace fift;
using namespace funC;
using namespace src;

namespace xfift {

    std::string strip(const std::string& s);
    void split(std::string s, char delim, std::vector<std::string>& res);
    std::string html_escape(const std::string& data);
    std::size_t path_complete(const fs::path& path, std::vector<std::string>& matches);

    struct XToken
    {
        std::string line;
        std::size_t begin_pos;
        std::size_t end_pos;

        std::string str() const;
        char prev_char() const;
        char next_char() const;

        XToken() = default;
        XToken(const char* token)
            : line(token)
            , begin_pos(0)
            , end_pos(line.size())
        {            
        }
    };

    XToken parse_token(
        const std::string& line, 
        std::size_t cursor_pos,
        const char* look_behind_chars,
        const char* look_ahead_chars
    );

    struct XResult
    {
        int code;
        std::string vmlog;
        std::string output;
        std::string ename;
        std::string evalue;
        std::vector<std::string> traceback;

        XResult();
        XResult(const std::string& msg);
        XResult(fift::IntError e);
        XResult(fift::Quit e);
        XResult(src::Fatal& fatal);
        XResult(src::ParseError& error);
        XResult(funC::UnifyError& unif_err);

        XResult vm_result();
    };
}