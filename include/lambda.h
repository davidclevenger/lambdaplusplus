#ifndef LAMBDA_H
#define LAMBDA_H
#include <functional>
#include <optional>
#include <type_traits>
#include <utility>

namespace lambda {

// Returns a callable that applies `g` first and then `f`, i.e. f(g(args...)).
template<typename F, typename G>
[[nodiscard]] constexpr auto
compose(F f, G g)
{
    return [f = std::move(f), g = std::move(g)]<typename... Args>(
             Args&&... args) -> decltype(auto) { return f(g(std::forward<Args>(args)...)); };
}

// Invokes `f(args...)`, swallowing any exception it throws.
//   - If `f` returns non-void, yields std::optional<R> (nullopt on throw).
//   - If `f` returns void, yields bool (false on throw).
template<typename F, typename... Args>
[[nodiscard]] auto
attempt(F&& f, Args&&... args)
{
    using R = std::invoke_result_t<F, Args...>;
    if constexpr (std::is_void_v<R>) {
        try {
            std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
            return true;
        } catch (...) {
            return false;
        }
    } else {
        try {
            return std::optional<R>(std::invoke(std::forward<F>(f), std::forward<Args>(args)...));
        } catch (...) {
            return std::optional<R>(std::nullopt);
        }
    }
}

// Wraps a callable into one that never propagates exceptions. The returned
// callable defers to attempt, so `nothrow(f)(args...)` behaves like
// `attempt(f, args...)`: non-void `f` yields std::optional<R>, void `f` yields
// bool. `f` is stored and invoked as const, so the wrapper can be reused.
template<typename F>
[[nodiscard]] auto
nothrow(F f)
{
    return [f = std::move(f)]<typename... Args>(Args&&... args) {
        return attempt(f, std::forward<Args>(args)...);
    };
}

namespace detail {

template<typename...>
inline constexpr bool always_false = false;

// Single parameter type of a non-generic, single-argument operator().
template<typename F>
struct call_arg;
template<typename C, typename R, typename A>
struct call_arg<R (C::*)(A)>
{
    using type = A;
};
template<typename C, typename R, typename A>
struct call_arg<R (C::*)(A) const>
{
    using type = A;
};
template<typename C, typename R, typename A>
struct call_arg<R (C::*)(A) noexcept>
{
    using type = A;
};
template<typename C, typename R, typename A>
struct call_arg<R (C::*)(A) const noexcept>
{
    using type = A;
};

// True (and exposes `type`) only when C has a non-template, single-argument
// operator() — i.e. a concrete lambda/functor. Generic callables such as
// [](auto&&){ ... } do not qualify and are treated as wildcards.
template<typename C, typename = void>
struct concrete_arg : std::false_type
{};
template<typename C>
struct concrete_arg<C, std::void_t<typename call_arg<decltype(&C::operator())>::type>>
  : std::true_type
{
    using type = typename call_arg<decltype(&C::operator())>::type;
};

// Strict case predicate: a concrete case matches only when its parameter type
// equals T ignoring cv/ref (no implicit conversions); a generic callable is a
// wildcard. Either way the case must be invocable with T.
template<typename Case, typename T>
constexpr bool
strict_case()
{
    using C = std::remove_cvref_t<Case>;
    if constexpr (concrete_arg<C>::value) {
        return std::is_same_v<std::remove_cvref_t<typename concrete_arg<C>::type>,
                              std::remove_cvref_t<T>> &&
               std::is_invocable_v<Case, T>;
    } else {
        return std::is_invocable_v<Case, T>;
    }
}

} // namespace detail

// Rust-like type match: invokes the first `case_` whose parameter type matches
// `value` exactly (cv/ref-insensitive), chosen at compile time via if constexpr
// (no std::variant / std::visit). Unlike overload resolution no implicit
// conversions are considered — a [](int) case will not catch a double. A
// generic callable such as [](auto&&){ ... } is a wildcard and should come
// last, like Rust's `_` arm.
template<typename T, typename Case, typename... Rest>
[[nodiscard]] constexpr decltype(auto)
match(T&& value, Case&& case_, Rest&&... rest)
{
    if constexpr (detail::strict_case<Case, T>()) {
        return std::invoke(std::forward<Case>(case_), std::forward<T>(value));
    } else if constexpr (sizeof...(Rest) > 0) {
        return match(std::forward<T>(value), std::forward<Rest>(rest)...);
    } else {
        static_assert(detail::always_false<T>,
                      "lambda::match: no case matches the value's exact type");
    }
}

} // namespace lambda

#endif // LAMBDA_H
