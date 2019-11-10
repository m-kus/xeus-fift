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
