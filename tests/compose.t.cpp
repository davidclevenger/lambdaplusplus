#include <lambda.h>

#include <memory>
#include <string>

#include <gtest/gtest.h>

TEST(ComposeTest, AppliesRightFunctionFirst)
{
    const auto add_one = [](int x) { return x + 1; };
    const auto times_two = [](int x) { return x * 2; };
    const auto f = lambda::compose(add_one, times_two);
    EXPECT_EQ(f(3), 7); // add_one(times_two(3)) == add_one(6) == 7
}

TEST(ComposeTest, ForwardsMultipleArguments)
{
    const auto sum = [](int a, int b) { return a + b; };
    const auto negate = [](int x) { return -x; };
    const auto f = lambda::compose(negate, sum);
    EXPECT_EQ(f(2, 5), -7); // negate(sum(2, 5)) == negate(7) == -7
}

TEST(ComposeTest, ChainsHeterogeneousTypes)
{
    const auto length = [](const std::string& s) { return s.size(); };
    const auto repeat = [](int n) { return std::string(static_cast<std::size_t>(n), 'x'); };
    const auto f = lambda::compose(length, repeat);
    EXPECT_EQ(f(4), 4u); // length(repeat(4)) == length("xxxx") == 4
}

TEST(ComposeTest, PerfectForwardsMoveOnlyArguments)
{
    const auto identity = [](int v) { return v; };
    const auto consume = [](std::unique_ptr<int> p) { return *p; };
    const auto f = lambda::compose(identity, consume);
    EXPECT_EQ(f(std::make_unique<int>(42)), 42); // move-only arg forwarded through
}

TEST(ComposeTest, IsUsableInConstantExpression)
{
    constexpr auto inc = [](int x) { return x + 1; };
    constexpr auto dbl = [](int x) { return x * 2; };
    constexpr auto f = lambda::compose(inc, dbl);
    static_assert(f(10) == 21, "inc(dbl(10)) == inc(20) == 21");
    EXPECT_EQ(f(10), 21);
}
