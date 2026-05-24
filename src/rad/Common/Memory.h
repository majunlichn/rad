#pragma once

#include <rad/Common/Platform.h>

#include <boost/align/aligned_allocator.hpp>

#include <memory>

namespace rad
{

// Allocate `size` bytes aligned to `alignment` bytes.
// - `size` will be rounded up to a multiple of `alignment`.
// - `alignment` must be a power of two.
void* AlignedAlloc(std::size_t size, std::size_t alignment);
// Free memory returned by `AlignedAlloc`.
void AlignedFree(void* p);

template <typename T, size_t Alignment>
using AlignedAllocator = boost::alignment::aligned_allocator<T, Alignment>;

} // namespace rad
