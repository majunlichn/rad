#include <rad/Core/Memory.h>

#include <gtest/gtest.h>

#include <array>
#include <cstdint>
#include <cstring>

namespace
{

bool IsAligned(void* ptr, std::size_t alignment)
{
    return reinterpret_cast<std::uintptr_t>(ptr) % alignment == 0;
}

} // namespace

TEST(Core, AlignedAlloc)
{
    // Allocations satisfy the requested alignment and remain writable.
    {
        constexpr std::array<std::size_t, 4> alignments = {8, 16, 32, 64};

        for (const std::size_t alignment : alignments)
        {
            void* ptr = rad::AlignedAlloc(257, alignment);
            ASSERT_NE(ptr, nullptr);
            EXPECT_TRUE(IsAligned(ptr, alignment));

            std::memset(ptr, 0xA5, 257);
            rad::AlignedFree(ptr);
        }
    }

    // Invalid allocation requests fail without returning storage.
    {
        EXPECT_EQ(rad::AlignedAlloc(0, 16), nullptr);
        EXPECT_EQ(rad::AlignedAlloc(16, 0), nullptr);
        EXPECT_EQ(rad::AlignedAlloc(16, 3), nullptr);
    }

    // Freeing a null pointer is a no-op.
    {
        rad::AlignedFree(nullptr);
    }
}
