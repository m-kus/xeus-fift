#include "xfift.hpp"

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
    auto offset = fift.code_complete("\"Asm.fif\" inc", 13, matches);

    ASSERT_EQ(1, matches.size());
    ASSERT_EQ("include", matches[0]);
    ASSERT_EQ(10, offset);
}

TEST(XFiftTest, SimpleInspect)
{
    xfift::XFift fift;
    fift.configure();

    std::string docstring = fift.code_inspect("include", 4);
    ASSERT_TRUE(docstring.size() > 50);
}

TEST(XFiftTest, EndLineInspect)
{
    xfift::XFift fift;
    fift.configure();

    std::string docstring = fift.code_inspect("four .s", 7);
    ASSERT_TRUE(docstring.size() > 50);
}