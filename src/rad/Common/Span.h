#pragma once

#include <rad/Common/Platform.h>

#include <cassert>

#include <initializer_list>
#include <ranges>
#include <span>
#include <type_traits>

namespace rad
{

// std::span extension with implicit adapters for function parameters (initializer_list,
// contiguous ranges, single elements, etc.). Not safe to store a Span; lifetime is bound
// to the caller's argument. Trivially copyable; pass by value.
template <typename T, size_t Extent = std::dynamic_extent>
class [[nodiscard]] Span : public std::span<T, Extent>
{
public:
    using Base = std::span<T, Extent>;

    using Base::span;

    constexpr Span(std::span<T, Extent> s) : Base(s) {}

    template <typename U>
        requires(!std::ranges::contiguous_range<U>)
    constexpr Span(const U& elem) : Base(&elem, 1)
    {
    }

    constexpr Span(std::initializer_list<std::remove_const_t<T>> list)
        requires(Extent == std::dynamic_extent)
        : Base(list.begin(), list.size())
    {
    }

    template <std::ranges::contiguous_range R>
        requires(Extent == std::dynamic_extent &&
                 !std::same_as<std::remove_cvref_t<R>, Span<T, Extent>> &&
                 std::convertible_to<std::remove_reference_t<std::ranges::range_reference_t<R>>,
                                     std::add_lvalue_reference_t<std::remove_cv_t<T>>>)
    constexpr Span(R&& range) : Base(std::ranges::data(range), std::ranges::size(range))
    {
    }
}; // class Span

template <typename T>
Span(std::initializer_list<T> list) -> Span<const T>;

template <typename T>
    requires(!std::ranges::contiguous_range<T>)
Span(const T& elem) -> Span<const T>;

template <std::ranges::contiguous_range R>
Span(R&& range) -> Span<
    std::remove_cv_t<std::remove_reference_t<std::ranges::range_reference_t<R>>>>;

template <typename T, size_t N>
constexpr auto MakeSpan(T (&arr)[N])
{
    return std::span<T>(arr, N);
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

    return std::span<T>{std::ranges::data(r), std::ranges::size(r)};
}

template <typename T>
constexpr auto MakeSpan(const std::initializer_list<T>& list)
{
    return std::span<const T>(list.begin(), list.size());
}

template <typename T>
    requires(!std::ranges::contiguous_range<T>)
constexpr auto MakeSpan(T& element)
{
    return std::span<T>(&element, 1);
}

// Binds to single const lvalues OR single temporaries (e.g. MakeSpan(42)).
// Temporaries will naturally become std::span<const T>.
template <typename T>
    requires(!std::ranges::contiguous_range<T>)
constexpr auto MakeSpan(const T& element)
{
    return std::span<const T>(&element, 1);
}

template <typename T>
constexpr auto MakeSpan(T* ptr, size_t size)
{
    return std::span<T>(ptr, size);
}

template <typename T>
constexpr auto MakeSpan(T* begin, T* end)
{
    assert(begin <= end);
    return std::span<T>(begin, static_cast<size_t>(end - begin));
}

} // namespace rad
