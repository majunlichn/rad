#pragma once

#include <rad/Common/Platform.h>

#include <cassert>

#include <initializer_list>
#include <ranges>
#include <span>
#include <type_traits>

namespace rad
{

template <typename T>
using Span = std::span<T, std::dynamic_extent>;

template <typename T, size_t N>
constexpr auto MakeSpan(T (&arr)[N])
{
    return Span<T>(arr, N);
}

// Accepts both lvalues and rvalues.
// If R is an rvalue (temporary) and not a explicitly borrowed range, forces the Span to be const T.
template <std::ranges::contiguous_range R>
auto MakeSpan(R&& r)
{
    using ElementType = std::remove_reference_t<std::ranges::range_reference_t<R>>;

    // If the range is a temporary, force 'const' so you can't mutate a dying object.
    using T = std::conditional_t<std::is_lvalue_reference_v<R> || std::ranges::borrowed_range<R>,
                                 ElementType, const ElementType>;

    return Span<T>{std::ranges::data(r), std::ranges::size(r)};
}

template <typename T>
constexpr auto MakeSpan(const std::initializer_list<T>& list)
{
    return Span<const T>(list.begin(), list.size());
}

template <typename T>
    requires(!std::ranges::contiguous_range<T>)
constexpr auto MakeSpan(T& element)
{
    return Span<T>(&element, 1);
}

// Binds to single const lvalues OR single temporaries (e.g. MakeSpan(42)).
// Temporaries will naturally become Span<const T>.
template <typename T>
    requires(!std::ranges::contiguous_range<T>)
constexpr auto MakeSpan(const T& element)
{
    return Span<const T>(&element, 1);
}

template <typename T>
constexpr auto MakeSpan(T* ptr, size_t size)
{
    return Span<T>(ptr, size);
}

template <typename T>
constexpr auto MakeSpan(T* begin, T* end)
{
    assert(begin <= end);
    return Span<T>(begin, std::distance(begin, end));
}

} // namespace rad
