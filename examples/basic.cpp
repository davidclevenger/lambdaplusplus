#include <iostream>
#include <string>

#include <lambda.h>

int
main()
{
    const auto add_one = [](int x) { return x + 1; };
    const auto times_two = [](int x) { return x * 2; };
    const auto transform = lambda::compose(add_one, times_two);

    std::cout << "compose(add_one, times_two)(3) = " << transform(3) << '\n';

    // attempt swallows exceptions: nullopt on throw, a value on success.
    const auto parsed = lambda::attempt([](const std::string& s) { return std::stoi(s); }, "123");
    std::cout << "attempt(stoi, \"123\") = " << parsed.value_or(-1) << '\n';

    const auto failed = lambda::attempt([](const std::string& s) { return std::stoi(s); }, "oops");
    std::cout << "attempt(stoi, \"oops\") has_value = " << std::boolalpha << failed.has_value()
              << '\n';
    return 0;
}
