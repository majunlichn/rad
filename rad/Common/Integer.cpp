#include <rad/Common/Integer.h>
#include <rad/Common/Platform.h>

#include <cassert>

#if defined(RAD_OS_WINDOWS)
#if defined(RAD_COMPILER_MSVC)
#include <intrin.h>
#endif
#endif

namespace rad
{

uint32_t BitScanReverse32Portable(uint32_t mask) noexcept
{
    assert(mask != 0);
#if defined(_WIN32)
    static_assert(sizeof(uint32_t) == sizeof(unsigned long));
    unsigned long index = 0;
    _BitScanReverse(&index, mask);
    return static_cast<uint32_t>(index);
#elif defined(__GNUC__) || defined(__clang__)
    static_assert(sizeof(mask) <= sizeof(unsigned int));
    return static_cast<uint32_t>(31 - __builtin_clz(static_cast<unsigned int>(mask)));
#else // fallback
    uint32_t index = 0;
    if (mask & 0xFFFF0000)
    {
        mask >>= 16;
        index += 16;
    }
    if (mask & 0xFF00)
    {
        mask >>= 8;
        index += 8;
    }
    if (mask & 0xF0)
    {
        mask >>= 4;
        index += 4;
    }
    if (mask & 0xC)
    {
        mask >>= 2;
        index += 2;
    }
    if (mask & 0x2)
    {
        mask >>= 1;
        index += 1;
    }
    return index;
#endif
}

uint32_t BitScanReverse64Portable(uint64_t mask) noexcept
{
    assert(mask != 0);
#if defined(_WIN64)
    static_assert(sizeof(uint64_t) <= sizeof(unsigned long long));
    unsigned long index = 0;
    _BitScanReverse64(&index, mask);
    return static_cast<uint32_t>(index);
#elif defined(_WIN32)
    static_assert(sizeof(uint32_t) <= sizeof(unsigned long));
    static_assert(sizeof(uint64_t) <= sizeof(unsigned long long));
    unsigned long index = 0;
    const uint32_t highPart = HighPart64(mask);
    _BitScanReverse(&index, (highPart != 0) ? highPart : LowPart64(mask));
    return (highPart != 0) ? (index + 32u) : index;
#elif defined(__GNUC__) || defined(__clang__)
    static_assert(sizeof(uint64_t) <= sizeof(unsigned long long));
    return static_cast<uint32_t>(63 - __builtin_clzll(static_cast<unsigned long long>(mask)));
#else // fallback
    uint32_t index = 0;
    // Check high 32 bits
    if (mask & 0xFFFFFFFF00000000ULL)
    {
        mask >>= 32;
        index += 32;
    }
    // Check high 16 bits
    if (mask & 0xFFFF0000)
    {
        mask >>= 16;
        index += 16;
    }
    // Check high 8 bits
    if (mask & 0xFF00)
    {
        mask >>= 8;
        index += 8;
    }
    // Check high 4 bits
    if (mask & 0xF0)
    {
        mask >>= 4;
        index += 4;
    }
    // Check high 2 bits
    if (mask & 0xC)
    {
        mask >>= 2;
        index += 2;
    }
    // Check the highest bit
    if (mask & 0x2)
    {
        mask >>= 1;
        index += 1;
    }
    return index;
#endif
}

} // namespace rad
