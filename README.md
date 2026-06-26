# lambdaplusplus

A small, header-only C++20 library of functional utilities (`compose`,
`attempt`, `match`).


[![CI](https://github.com/<owner>/lambdaplusplus/actions/workflows/ci.yml/badge.svg)](https://github.com/<owner>/lambdaplusplus/actions/workflows/ci.yml)


## Usage

```cpp
#include <lambda.h>

#include <iostream>
#include <string>

int main() {
    auto add_one   = [](int x) { return x + 1; };
    auto times_two = [](int x) { return x * 2; };
    auto f = lambda::compose(add_one, times_two);  // add_one(times_two(x))
    std::cout << f(3) << '\n';                      // -> 7

    // attempt runs a callable and swallows exceptions: optional<R> for
    // value-returning callables (nullopt on throw), bool for void callables.
    auto parsed = lambda::attempt([](const std::string& s) { return std::stoi(s); }, "41");
    std::cout << parsed.value_or(-1) << '\n';       // -> 41
}
```

The public API — all in `<lambda.h>`, namespace `lambda`:

| Function | Description |
| --- | --- |
| `compose(f, g)` | Returns a callable computing `f(g(args...))`. |
| `attempt(f, args...)` | Invokes `f(args...)`, swallowing exceptions — `std::optional<R>`, or `bool` when `f` returns void. |
| `nothrow(f)` | Wraps `f` into a reusable non-throwing callable; `nothrow(f)(args...)` behaves like `attempt(f, args...)`. |
| `match(x, cases...)` | Invokes the first case whose parameter type matches `x` exactly — no implicit conversions, selected at compile time via `if constexpr`. Put a generic `[](auto&&){…}` last as the catch-all (`_`). |

### Consuming from another CMake project

After installing (`cmake --install build`):

```cmake
find_package(lambdaplusplus REQUIRED)
target_link_libraries(my_app PRIVATE lambdaplusplus::lambdaplusplus)
```

Or vendor it directly as a subdirectory:

```cmake
add_subdirectory(external/lambdaplusplus)
target_link_libraries(my_app PRIVATE lambdaplusplus::lambdaplusplus)
```

## License

[MIT](LICENSE) © 2026 David Clevenger
