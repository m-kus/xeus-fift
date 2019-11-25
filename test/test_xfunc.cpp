#include "xfunc/xfunc.hpp"

#include <gtest/gtest.h>

TEST(XFuncTest, TryOut)
{
    xfunc::XFunc func;
    func.configure();

    auto res = func.do_parse("forall X -> () print(X a) impure asm \"s0 PRINT\";");
    std::clog << res.evalue << std::endl;
    ASSERT_EQ(0, res.code);

    auto res2 = func.do_parse("int i = 2; print(i)");
    std::cout << res2.evalue << "\n";
    ASSERT_EQ(0, res2.code);

    auto res3 = func.do_interpret();
    std::clog << res3.evalue << std::endl;
    std::clog << res3.vmlog << std::endl;
    ASSERT_EQ(0, res3.code);
    ASSERT_EQ("2", res3.output);
}