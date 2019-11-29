#include "xfift/xfift.hpp"

#include <gtest/gtest.h>

TEST(XFiftTest, EmptyOutput)
{
    xfift::XFift fift;
    fift.configure();

    auto res = fift.do_interpret("1");
    ASSERT_EQ(0, res.code);
    ASSERT_EQ("", res.output);
}

TEST(XFiftTest, SimpleExpression)
{
    xfift::XFift fift;
    fift.configure();

    auto res = fift.do_interpret("1 2 + .");
    ASSERT_EQ(0, res.code);
    ASSERT_EQ("3", res.output);
}

TEST(XFiftTest, MultiRequest)
{
    xfift::XFift fift;
    fift.configure();

    auto res = fift.do_interpret("1 2");
    ASSERT_EQ(0, res.code);
    ASSERT_EQ("", res.output);

    auto res1 = fift.do_interpret("+ .");
    ASSERT_EQ(0, res1.code);
    ASSERT_EQ("3", res1.output);
}

TEST(XFiftTest, SimpleComplete)
{
    xfift::XFift fift;
    fift.configure();

    std::vector<std::string> matches;
    xfift::XToken token = fift.code_complete("\"Asm.fif\" inc", 13, matches);
    ASSERT_EQ("inc", token.str());

    ASSERT_EQ(1, matches.size());
    ASSERT_EQ("include", matches[0]);
}

TEST(XFiftTest, SimpleInspect)
{
    xfift::XFift fift;
    fift.configure();

    std::string docstring;
    fift.code_inspect("include", 3, docstring);
    ASSERT_TRUE(docstring.size() > 50);
}

TEST(XFiftTest, EndLineInspect)
{
    auto token = xfift::parse_token("four .s", 7, " \"", " \"");
    ASSERT_EQ(".s", token.str());
}

TEST(XFiftTest, EndWordInspect)
{
    auto token = xfift::parse_token("tuple ", 5, " \"", " \"");
    ASSERT_EQ("tuple", token.str());
}

TEST(XFiftTest, QuoteInspect)
{
    auto token = xfift::parse_token("abort\"message\"", 2, " \"", " \"");
    ASSERT_EQ("abort", token.str());
}