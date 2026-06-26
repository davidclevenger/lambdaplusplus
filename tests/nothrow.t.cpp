#include <lambda.h>

#include <optional>
#include <stdexcept>
#include <string>
#include <type_traits>

#include <gtest/gtest.h>

TEST(NothrowTest, WrapsValueReturningCallable)
{
    const auto safe_inc = lambda::nothrow([](int x) { return x + 1; });
    const auto r = safe_inc(41);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(*r, 42);
}

TEST(NothrowTest, ReturnsNulloptWhenWrappedThrows)
{
    const auto safe_parse = lambda::nothrow([](const std::string& s) { return std::stoi(s); });
    EXPECT_FALSE(safe_parse("not a number").has_value());
    EXPECT_EQ(safe_parse("123").value_or(-1), 123);
}

TEST(NothrowTest, WrappedCallableIsReusable)
{
    const auto safe_div = lambda::nothrow([](int a, int b) {
        if (b == 0)
            throw std::domain_error("divide by zero");
        return a / b;
    });
    EXPECT_EQ(safe_div(10, 2).value_or(-1), 5);
    EXPECT_FALSE(safe_div(10, 0).has_value());
    EXPECT_EQ(safe_div(9, 3).value_or(-1), 3); // reusable: state-free across calls
}

TEST(NothrowTest, WrapsVoidCallable)
{
    int calls = 0;
    const auto safe_run = lambda::nothrow([&calls](bool should_throw) {
        if (should_throw)
            throw std::runtime_error("boom");
        ++calls;
    });
    EXPECT_TRUE(safe_run(false));
    EXPECT_FALSE(safe_run(true));
    EXPECT_EQ(calls, 1);
}

TEST(NothrowTest, ForwardsArgumentsToWrappedCallable)
{
    const auto safe_mul = lambda::nothrow([](int a, int b, int c) { return a * b * c; });
    EXPECT_EQ(safe_mul(2, 3, 4).value_or(0), 24);
}

TEST(NothrowTest, BehavesLikeAttempt)
{
    const auto f = [](int x) -> int {
        if (x < 0)
            throw std::out_of_range("negative");
        return x * x;
    };
    const auto safe_f = lambda::nothrow(f);
    EXPECT_EQ(safe_f(4), lambda::attempt(f, 4));   // both std::optional<int>{16}
    EXPECT_EQ(safe_f(-1), lambda::attempt(f, -1)); // both std::nullopt
}

TEST(NothrowTest, DeducesResultTypePerCall)
{
    const auto safe_id = lambda::nothrow([](auto x) { return x; });
    const auto i = safe_id(7);
    const auto s = safe_id(std::string("hi"));
    static_assert(std::is_same_v<decltype(i), const std::optional<int>>);
    static_assert(std::is_same_v<decltype(s), const std::optional<std::string>>);
    EXPECT_EQ(i.value_or(-1), 7);
    EXPECT_EQ(s.value_or(""), "hi");
}
