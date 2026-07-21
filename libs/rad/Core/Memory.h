#pragma once

#include <cstddef>

namespace rad
{

// Allocates size bytes aligned to a non-zero power-of-two alignment; returns nullptr on failure.
[[nodiscard]] void* AlignedAlloc(std::size_t size, std::size_t alignment) noexcept;
// Frees memory returned by AlignedAlloc; ptr may be nullptr.
void AlignedFree(void* ptr) noexcept;

} // namespace rad
