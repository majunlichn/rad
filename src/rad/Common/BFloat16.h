#pragma once

#include <rad/Common/Platform.h>

#include <rad/Common/Float.h>

#include <bit>

namespace rad
{

// The bfloat16 format was developed by Google Brain, an artificial intelligence research group at Google.
// https://en.wikipedia.org/wiki/Bfloat16_floating-point_format

// https://github.com/pytorch/pytorch/blob/main/torch/headeronly/util/BFloat16.h

inline float bf16_to_fp32(uint16_t src)
{
    return fp32_from_bits(static_cast<uint32_t>(src) << 16);
}

inline uint16_t bf16_from_fp32_round_to_zero(float src)
{
    return static_cast<uint16_t>(fp32_to_bits(src) >> 16);
}

inline uint16_t bf16_from_fp32_round_to_nearest_even(float src)
{
#if defined(__HIPCC__)
    if (src != src)
#else
    if (std::isnan(src))
#endif
    {
        return UINT16_C(0x7FC0);
    }
    else
    {
        const uint32_t U32 = std::bit_cast<uint32_t>(src);
        uint32_t rounding_bias = ((U32 >> 16) & 1) + UINT32_C(0x7FFF);
        return static_cast<uint16_t>((U32 + rounding_bias) >> 16);
    }
}

#if defined(RAD_COMPILER_GCC) || defined(RAD_COMPILER_CLANG)

using BFloat16 = __bf16;

#else

struct alignas(2) BFloat16
{
    uint16_t m_bits;
    BFloat16() = default;
    BFloat16(float value) :
        m_bits(bf16_from_fp32_round_to_nearest_even(value))
    {
    }

    operator float() const { return bf16_to_fp32(m_bits); }

    BFloat16& operator=(float rhs)
    {
        m_bits = bf16_from_fp32_round_to_nearest_even(rhs);
        return *this;
    }
    BFloat16& operator+=(float rhs)
    {
        m_bits = bf16_from_fp32_round_to_nearest_even(bf16_to_fp32(m_bits) + rhs);
        return *this;
    }
    BFloat16& operator-=(float rhs)
    {
        m_bits = bf16_from_fp32_round_to_nearest_even(bf16_to_fp32(m_bits) - rhs);
        return *this;
    }
    BFloat16& operator*=(float rhs)
    {
        m_bits = bf16_from_fp32_round_to_nearest_even(bf16_to_fp32(m_bits) * rhs);
        return *this;
    }
    BFloat16& operator/=(float rhs)
    {
        m_bits = bf16_from_fp32_round_to_nearest_even(bf16_to_fp32(m_bits) / rhs);
        return *this;
    }

    friend BFloat16 operator+(BFloat16 lhs, BFloat16 rhs)
    {
        return BFloat16(static_cast<float>(lhs) + static_cast<float>(rhs));
    }

    friend BFloat16 operator-(BFloat16 lhs, BFloat16 rhs)
    {
        return BFloat16(static_cast<float>(lhs) - static_cast<float>(rhs));
    }

    friend BFloat16 operator*(BFloat16 lhs, BFloat16 rhs)
    {
        return BFloat16(static_cast<float>(lhs) * static_cast<float>(rhs));
    }

    friend BFloat16 operator/(BFloat16 lhs, BFloat16 rhs)
    {
        return BFloat16(static_cast<float>(lhs) / static_cast<float>(rhs));
    }

}; // struct BFloat16

#endif

static_assert(sizeof(BFloat16) == 2);

} // namespace rad
