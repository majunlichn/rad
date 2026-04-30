#pragma once

#include <cassert>
#include <climits>
#include <cstddef>
#include <cstdint>

#include <bit>
#include <concepts>
#include <limits>
#include <type_traits>

namespace rad
{

using Sint8 = std::int8_t;
using Sint16 = std::int16_t;
using Sint32 = std::int32_t;
using Sint64 = std::int64_t;

using Uint8 = std::uint8_t;
using Uint16 = std::uint16_t;
using Uint32 = std::uint32_t;
using Uint64 = std::uint64_t;

template <typename T>
constexpr size_t BitCount = sizeof(T) * CHAR_BIT;

template <typename T>
    requires std::unsigned_integral<T> && (BitCount<T> == 32)
[[nodiscard]] constexpr uint16_t HighPart32(T value) noexcept
{
    return static_cast<uint16_t>(value >> 16);
}

template <typename T>
    requires std::unsigned_integral<T> && (BitCount<T> == 32)
[[nodiscard]] constexpr uint16_t LowPart32(T value) noexcept
{
    return static_cast<uint16_t>(value & 0xFFFF);
}

template <typename T>
    requires std::unsigned_integral<T> && (BitCount<T> == 64)
[[nodiscard]] constexpr uint32_t HighPart64(T value) noexcept
{
    return static_cast<uint32_t>(value >> 32);
}

template <typename T>
    requires std::unsigned_integral<T> && (BitCount<T> == 64)
[[nodiscard]] constexpr uint32_t LowPart64(T value) noexcept
{
    return static_cast<uint32_t>(value & 0xFFFFFFFF);
}

template <std::unsigned_integral T>
[[nodiscard]] constexpr bool HasBits(T mask, T bits) noexcept
{
    return ((mask & bits) == bits);
}

template <std::unsigned_integral T>
[[nodiscard]] constexpr bool HasNoBits(T mask, T bits) noexcept
{
    return ((mask & bits) == 0);
}

template <std::unsigned_integral T>
[[nodiscard]] constexpr auto SignExtend(T value, size_t bits) noexcept
{
    assert((bits > 0) && bits <= BitCount<T>);
    T mask = T(1) << (bits - 1);
    T extended = (value ^ mask) - mask;
    return static_cast<std::make_signed_t<T>>(extended);
}

template <std::unsigned_integral T>
[[nodiscard]] constexpr bool HasAnyBits(T mask, T bits) noexcept
{
    return ((mask & bits) != 0);
}

// Bits are counted from the LSB.
template <std::unsigned_integral T>
[[nodiscard]] constexpr T ExtractBits(T value, size_t start, size_t count) noexcept
{
    assert((start >= 0) && (count >= 0));
    assert(start + count <= BitCount<T>);
    if (count == 0) [[unlikely]]
    {
        return 0;
    }
    if (count == BitCount<T>) [[unlikely]]
    {
        return value;
    }
    T mask = ((T(1) << count) - 1);
    return (value >> start) & mask;
}

// Bits are counted from the LSB.
template <std::unsigned_integral T>
[[nodiscard]] constexpr T ReplaceBits(T dest, T src, size_t start, size_t count) noexcept
{
    assert(start >= 0 && count >= 0);
    assert(start + count <= BitCount<T>);
    if (count == 0) [[unlikely]]
    {
        return dest;
    }
    if (count == BitCount<T>) [[unlikely]]
    {
        return src;
    }
    T mask = ((T(1) << count) - 1) << start;
    return (dest & ~mask) | ((src << start) & mask);
}

template <std::unsigned_integral T>
[[nodiscard]] constexpr int BitScanReverse(T value) noexcept
{
    assert(value != 0);
    return std::bit_width(value) - 1;
}

// Search the mask data from most significant bit (MSB) to least significant bit (LSB) for a set bit
// (1). The mask input must be nonzero or the index returned is undefined. Returns index in [0, 31],
// 0 = least significant bit, 31 = most significant.
template <typename T>
    requires std::unsigned_integral<T> && (BitCount<T> == 32)
[[nodiscard]] constexpr int BitScanReversePortable(T mask) noexcept
{
    int index = 0;
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
}

// Returns index in [0, 63], 0 = least significant bit, 63 = most significant.
template <typename T>
    requires std::unsigned_integral<T> && (BitCount<T> == 64)
[[nodiscard]] constexpr int BitScanReversePortable(T mask) noexcept
{
    int index = 0;
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
}

template <std::unsigned_integral T>
[[nodiscard]] constexpr int CountBits(T value) noexcept
{
    return std::popcount(value);
}

template <typename T>
    requires std::unsigned_integral<T> && (BitCount<T> == 32)
[[nodiscard]] constexpr int CountBitsPortable(T x) noexcept
{
    // https://en.wikipedia.org/wiki/Hamming_weight
    x = x - ((x >> 1) & 0x55555555);
    x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
    x = (((x + (x >> 4)) & 0x0F0F0F0F) * 0x01010101) >> (24);
    return static_cast<int>(x);
}

template <typename T>
    requires std::unsigned_integral<T> && (BitCount<T> == 64)
[[nodiscard]] constexpr int CountBitsPortable(T x) noexcept
{
    // https://en.wikipedia.org/wiki/Hamming_weight
    const T m1 = 0x5555555555555555;  // binary: 0101...
    const T m2 = 0x3333333333333333;  // binary: 00110011..
    const T m4 = 0x0f0f0f0f0f0f0f0f;  // binary:  4 zeros,  4 ones ...
    const T m8 = 0x00ff00ff00ff00ff;  // binary:  8 zeros,  8 ones ...
    const T m16 = 0x0000ffff0000ffff; // binary: 16 zeros, 16 ones ...
    const T m32 = 0x00000000ffffffff; // binary: 32 zeros, 32 ones
    const T h01 = 0x0101010101010101; // the sum of 256 to the power of 0,1,2,3,...
    x -= (x >> 1) & m1;               // put count of each 2 bits into those 2 bits
    x = (x & m2) + ((x >> 2) & m2);   // put count of each 4 bits into those 4 bits
    x = (x + (x >> 4)) & m4;          // put count of each 8 bits into those 8 bits
    return (x * h01) >> 56;           // returns left 8 bits of x + (x<<8) + (x<<16) + (x<<24) + ...
}

template <typename T>
    requires std::unsigned_integral<T> && (BitCount<T> == 32)
[[nodiscard]] constexpr T ReverseBits(T n) noexcept
{
    n = (n << 16) | (n >> 16);
    n = ((n & 0x00ff00ff) << 8) | ((n & 0xff00ff00) >> 8);
    n = ((n & 0x0f0f0f0f) << 4) | ((n & 0xf0f0f0f0) >> 4);
    n = ((n & 0x33333333) << 2) | ((n & 0xcccccccc) >> 2);
    n = ((n & 0x55555555) << 1) | ((n & 0xaaaaaaaa) >> 1);
    return n;
}

template <typename T>
    requires std::unsigned_integral<T> && (BitCount<T> == 64)
[[nodiscard]] constexpr T ReverseBits(T n) noexcept
{
    const T lo = ReverseBits(static_cast<uint32_t>(n));
    const T hi = ReverseBits(static_cast<uint32_t>(n >> 32));
    return (lo << 32) | hi;
}

template <std::unsigned_integral T>
[[nodiscard]] constexpr bool IsPow2(T x) noexcept
{
#if defined(__cpp_lib_int_pow2)
    return std::has_single_bit(x);
#else // fallback
    return (x != 0) && ((x & (x - 1)) == 0);
#endif
}

template <std::unsigned_integral T>
[[nodiscard]] constexpr T Pow2AlignUp(T value, T alignment) noexcept
{
    assert(IsPow2(alignment));
    assert(value <= std::numeric_limits<T>::max() - (alignment - 1)); // check overflow
    return ((value + (alignment - 1)) & ~(alignment - 1));
}

template <std::unsigned_integral T>
[[nodiscard]] constexpr T Pow2AlignDown(T value, T alignment) noexcept
{
    assert(IsPow2(alignment));
    return (value & ~(alignment - 1));
}

template <std::unsigned_integral T>
[[nodiscard]] constexpr T RoundUpToMultiple(T value, T alignment) noexcept
{
    assert(alignment >= 1);
    assert(value <= std::numeric_limits<T>::max() - (alignment - 1)); // check overflow
    if (IsPow2(alignment))
    {
        return Pow2AlignUp(value, alignment);
    }
    else
    {
        return (((value + (alignment - 1)) / alignment) * alignment);
    }
}

template <std::unsigned_integral T>
[[nodiscard]] constexpr T RoundDownToMultiple(T value, T alignment) noexcept
{
    assert(alignment >= 1);
    if (IsPow2(alignment))
    {
        return Pow2AlignDown(value, alignment);
    }
    else
    {
        return ((value / alignment) * alignment);
    }
}

template <std::unsigned_integral T>
[[nodiscard]] constexpr T DivRoundUp(T a, T b) noexcept
{
    assert(b > 0);
    if (a == 0) [[unlikely]]
    {
        return 0;
    }
    return (a - 1) / b + 1;
}

// Round up to the next power of 2.
// The input must be less than the maximum power of 2 representable by the type, otherwise the
// result is undefined. If x is already a power of 2, the result will be the next power of 2,
// RoundUpToNextPow2(16) will return 32. If x is 0, the result will be 1.
template <std::unsigned_integral T>
[[nodiscard]] constexpr T RoundUpToNextPow2(T x) noexcept
{
    // Pre: x < 2^(digits-1), so the next power of two fits in T.
    assert(x < (T(1) << (BitCount<T> - 1)));
    return T(1) << std::bit_width(x);
}

// Returns the smallest integral power of two that is not smaller than x.
template <std::unsigned_integral T>
[[nodiscard]] constexpr T RoundUpToPow2(T x) noexcept
{
    // Pre: x <= 2^(digits-1), so the next power of two fits in T.
    assert(x <= (T(1) << (BitCount<T> - 1)));
    return std::bit_ceil(x);
}

// If x is not zero, calculates the largest integral power of two that is not greater than x.
// If x is zero, returns zero.
template <std::unsigned_integral T>
[[nodiscard]] constexpr T RoundDownToPow2(T x) noexcept
{
    return std::bit_floor(x);
}

template <typename T>
    requires std::unsigned_integral<T> && (BitCount<T> == 32)
[[nodiscard]] constexpr T RoundUpToNextPow2Portable(T x) noexcept
{
    assert(x < 0x80000000u);
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return x + 1;
}

template <typename T>
    requires std::unsigned_integral<T> && (BitCount<T> == 64)
[[nodiscard]] constexpr T RoundUpToNextPow2Portable(T x) noexcept
{
    assert(x < 0x8000000000000000u);
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    x |= x >> 32;
    return x + 1;
}

template <typename T>
    requires std::unsigned_integral<T> && (BitCount<T> == 32)
[[nodiscard]] constexpr T RoundUpToPow2Portable(T x) noexcept
{
    assert(x <= 0x80000000u);
    if (x > 1)
    {
        return RoundUpToNextPow2Portable(x - 1);
    }
    else
    {
        return 1;
    }
}

template <typename T>
    requires std::unsigned_integral<T> && (BitCount<T> == 64)
[[nodiscard]] constexpr T RoundUpToPow2Portable(T x) noexcept
{
    assert(x <= 0x8000000000000000u);
    if (x > 1)
    {
        return RoundUpToNextPow2Portable(x - 1);
    }
    else
    {
        return 1;
    }
}

} // namespace rad
