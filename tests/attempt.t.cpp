#include <lambda.h>

#include <optional>
#include <stdexcept>
#include <type_traits>

#include <gtest/gtest.h>

namespace {

struct Widget
{
    int value;
    int add(int x) const { return value + x; }
};

} // namespace

TEST(AttemptTest, ReturnsValueWhenCallableSucceeds)
{
    const auto r = lambda::attempt([](int x) { return x + 1; }, 41);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(*r, 42);
}

TEST(AttemptTest, ReturnsNulloptWhenCallableThrows)
{
    const auto r = lambda::attempt(
      [](int x) -> int {
          if (x < 0)
              throw std::runtime_error("negative");
          return x;
      },
      -1);
    EXPECT_FALSE(r.has_value());
}

TEST(AttemptTest, CatchesNonStandardExceptions)
{
    const auto r = lambda::attempt([]() -> int { throw 42; });
    EXPECT_FALSE(r.has_value());
}

TEST(AttemptTest, ForwardsArgumentsToCallable)
{
    const auto r = lambda::attempt([](int a, int b) { return a * b; }, 6, 7);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(*r, 42);
}

TEST(AttemptTest, InvokesMemberFunctionPointer)
{
    const Widget w{ 10 };
    const auto r = lambda::attempt(&Widget::add, w, 5); // std::invoke -> w.add(5)
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(*r, 15);
}

TEST(AttemptTest, ReturnsTrueWhenVoidCallableSucceeds)
{
    int side_effect = 0;
    const bool ok = lambda::attempt([&] { side_effect = 1; });
    EXPECT_TRUE(ok);
    EXPECT_EQ(side_effect, 1);
}

TEST(AttemptTest, ReturnsFalseWhenVoidCallableThrows)
{
    const bool ok = lambda::attempt([] { throw std::runtime_error("boom"); });
    EXPECT_FALSE(ok);
}

TEST(AttemptTest, ReturnTypeDependsOnCallableResult)
{
    static_assert(std::is_same_v<decltype(lambda::attempt([] { return 1; })), std::optional<int>>);
    static_assert(std::is_same_v<decltype(lambda::attempt([] {})), bool>);
    SUCCEED();
}
