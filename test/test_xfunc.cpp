#include "xfunc/xfunc.hpp"

#include <gtest/gtest.h>

TEST(XFuncTest, TryOut)
{
    xfift::XFunc func;
    func.configure();

    auto res0 = func.do_interpret("forall X -> () print(X a) impure asm \"s0 PRINT\";");
    std::clog << res0.evalue << std::endl;
    ASSERT_EQ(0, res0.code);

    auto res1 = func.do_interpret("int i = 2; print(i)");
    std::cout << res1.evalue << std::endl;
    std::clog << res1.vmlog << std::endl;
    ASSERT_EQ(0, res1.code);
    ASSERT_EQ("2", res1.output);
}

TEST(XFuncTest, AutoReturn)
{
    xfift::XFunc func;
    func.configure();

    auto res = func.do_interpret("int i = 42");
    std::cout << res.evalue << std::endl;
    std::clog << res.vmlog << std::endl;
    ASSERT_EQ(0, res.code);
    ASSERT_EQ("42", res.output);
}