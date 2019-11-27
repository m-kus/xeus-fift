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

    xfift::XToken token = fift.parse_token("\"Asm.fif\" inc", 13);
    ASSERT_EQ("inc", token.str());

    std::vector<std::string> matches;
    fift.code_complete(token, matches);

    ASSERT_EQ(1, matches.size());
    ASSERT_EQ("include", matches[0]);
}

TEST(XFiftTest, SimpleInspect)
{
    xfift::XFift fift;
    fift.configure();

    std::string docstring = fift.code_inspect("include");
    ASSERT_TRUE(docstring.size() > 50);
}

TEST(XFiftTest, EndLineInspect)
{
    xfift::XFift fift;
    fift.configure();

    auto token = fift.parse_token("four .s", 7);
    ASSERT_EQ(".s", token.str());
}

TEST(XFiftTest, EndWordInspect)
{
    xfift::XFift fift;
    fift.configure();

    auto token = fift.parse_token("tuple ", 5);
    ASSERT_EQ("tuple", token.str());
}

TEST(XFiftTest, QuoteInspect)
{
    xfift::XFift fift;
    fift.configure();

    auto token = fift.parse_token("abort\"message\"", 2);
    ASSERT_EQ("abort", token.str());
}