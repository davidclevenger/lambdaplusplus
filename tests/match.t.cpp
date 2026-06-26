#include <lambda.h>

#include <string>
#include <utility>

#include <gtest/gtest.h>

namespace {
struct Unknown
{};
} // namespace

TEST(MatchTest, SelectsCaseByValueType)
{
    const auto on_int = [](int x) { return x * 2; };
    const auto on_string = [](const std::string& s) { return static_cast<int>(s.size()); };
    EXPECT_EQ(lambda::match(21, on_int, on_string), 42);
    EXPECT_EQ(lambda::match(std::string("hello"), on_int, on_string), 5);
}

TEST(MatchTest, BindsTheMatchedValue)
{
    EXPECT_EQ(lambda::match(20, [](int x) { return x + 22; }), 42);
}

TEST(MatchTest, FirstMatchingCaseWins)
{
    // Both cases accept an int; the first listed one wins (Rust arm order).
    const auto r = lambda::match(7, [](int) { return 1; }, [](auto) { return 2; });
    EXPECT_EQ(r, 1);
}

TEST(MatchTest, GenericCaseActsAsCatchAll)
{
    const auto classify = [](auto&& v) {
        return lambda::match(
          std::forward<decltype(v)>(v),
          [](int) { return std::string("int"); },
          [](const std::string&) { return std::string("string"); },
          [](auto&&) { return std::string("other"); });
    };
    EXPECT_EQ(classify(1), "int");
    EXPECT_EQ(classify(std::string("x")), "string");
    EXPECT_EQ(classify(Unknown{}), "other");
}

TEST(MatchTest, PreservesValueCategoryOfArgument)
{
    int n = 1;
    lambda::match(n, [](int& x) { x = 99; }); // case takes lvalue ref, mutates original
    EXPECT_EQ(n, 99);
}

TEST(MatchTest, ReturnsReferenceWhenCaseDoes)
{
    int storage = 5;
    int& ref = lambda::match(storage, [](int& x) -> int& { return x; }); // decltype(auto)
    ref = 123;
    EXPECT_EQ(storage, 123);
}

TEST(MatchTest, DoesNotMatchViaImplicitConversion)
{
    // Strict: a double is not routed to the int case (no int <- double conversion).
    const auto r = lambda::match(
      3.14,
      [](int) { return std::string("int"); },
      [](double) { return std::string("double"); },
      [](auto&&) { return std::string("other"); });
    EXPECT_EQ(r, "double");
}

TEST(MatchTest, DistinctArithmeticTypesAreSeparateArms)
{
    const auto label = [](auto&& v) {
        return lambda::match(
          std::forward<decltype(v)>(v),
          [](int) { return std::string("int"); },
          [](long) { return std::string("long"); },
          [](auto&&) { return std::string("other"); });
    };
    EXPECT_EQ(label(1), "int");
    EXPECT_EQ(label(1L), "long");
    EXPECT_EQ(label(static_cast<short>(1)), "other"); // short matches neither arm strictly
}
