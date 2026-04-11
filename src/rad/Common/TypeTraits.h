#pragma once

#include <rad/Common/Platform.h>

#include <cstdint>

#include <bit> // for std::bit_cast
#include <concepts>
#include <type_traits>

namespace rad
{

template <class T, class... Types>
constexpr bool is_any_of = (std::is_same_v<T, Types> || ...);

template <typename T, typename... Types>
constexpr bool are_all_same = (std::is_same_v<T, Types> && ...);

template <class T>
concept Enumeration = std::is_enum_v<T>;

template <class T>
concept TriviallyCopyable = std::is_trivially_copyable_v<T>;

template <Enumeration T>
constexpr auto UnderlyingCast(T t) noexcept
{
    return static_cast<std::underlying_type_t<T>>(t);
}

#if defined(__cpp_lib_bit_cast)

using std::bit_cast;

#else

template <class To, class From>
    requires(sizeof(To) == sizeof(From) && TriviallyCopyable<To> && TriviallyCopyable<From>)
constexpr To bit_cast(const From& src) noexcept
{
#if __has_builtin(__builtin_bit_cast) || defined(_MSC_VER)
    return __builtin_bit_cast(To, src);
#else
    static_assert(std::is_trivially_constructible_v<To>,
                  "bit_cast fallback requires trivially constructible destination type");
    To dst;
    std::memcpy(&dst, &src, sizeof(To));
    return dst;
#endif
}

#endif

} // namespace rad
