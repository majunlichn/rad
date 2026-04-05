#pragma once

#include <ranges>
#include <range/v3/all.hpp>
#include <execution>

namespace rad
{

// Python-like slicing of random access ranges, with support for negative indices and steps.
// Negative steps reverse the order of the slice.
template <std::ranges::random_access_range Range>
[[nodiscard]] auto Slice(Range&& r, ptrdiff_t start, ptrdiff_t stop, ptrdiff_t step = 1)
{
    using Container = std::remove_cvref_t<Range>;
    const auto size = std::ranges::size(r);
    if ((size == 0) || (step == 0))
    {
        return Container{};
    }
    start = (start < 0) ? (size + start) : start;
    start = std::clamp<ptrdiff_t>(start, 0, size);
    stop = (stop < 0) ? (size + stop) : stop;
    stop = std::clamp<ptrdiff_t>(stop, -1, size);
    if (step > 0)
    {
        if (start >= stop)
        {
            return Container{};
        }
        auto sliced = std::forward<Range>(r) |
            ranges::views::slice(start, stop) |
            ranges::views::stride(step);
        return ranges::to<Container>(sliced);
    }
    else
    {
        if (start <= stop)
        {
            return Container{};
        }
        auto sliced = std::forward<Range>(r) |
            ranges::views::slice(stop + 1, start + 1) |
            ranges::views::reverse |
            ranges::views::stride(-step);
        return ranges::to<Container>(sliced);
    }
}

template<std::ranges::random_access_range Range,
    typename ExecutionPolicy = std::execution::sequenced_policy,
    typename Compare = std::ranges::less>
[[nodiscard]] std::vector<size_t>
SortIndices(Range&& r, ExecutionPolicy policy = std::execution::seq, Compare comp = {})
{
    const auto count = std::ranges::size(r);
    if (count == 0) return {};
    std::vector<size_t> indices(count);
    std::iota(indices.begin(), indices.end(), size_t{ 0 });
    auto it = std::ranges::begin(r); // Use iterator indexing, guaranteed by random_access_range
    std::sort(policy, indices.begin(), indices.end(), [&](size_t i, size_t j) { return comp(it[i], it[j]); });
    return indices;
}

template<std::ranges::random_access_range Range,
    typename ExecutionPolicy = std::execution::sequenced_policy,
    typename Compare = std::ranges::less>
[[nodiscard]] std::vector<size_t>
StableSortIndices(Range&& r, ExecutionPolicy policy = std::execution::seq, Compare comp = {})
{
    const auto count = std::ranges::size(r);
    if (count == 0) return {};
    std::vector<size_t> indices(count);
    std::iota(indices.begin(), indices.end(), size_t{ 0 });
    auto it = std::ranges::begin(r); // Use iterator indexing, guaranteed by random_access_range
    std::stable_sort(policy, indices.begin(), indices.end(), [&](size_t i, size_t j) { return comp(it[i], it[j]); });
    return indices;
}

} // namespace rad
