#pragma once

#include <rad/Common/Integer.h>
#include <rad/Common/Platform.h>

// C Headers
#include <cassert>
#include <cfloat>

// C++ Headers
#include <algorithm>
#include <bit>
#include <cmath>
#include <numbers>

namespace rad
{

// https://github.com/pytorch/pytorch/blob/main/torch/headeronly/util/floating_point_utils.h
inline float fp32_from_bits(uint32_t w)
{
#if defined(__OPENCL_VERSION__)
    return as_float(w);
#elif defined(__CUDA_ARCH__) || defined(__HIP_DEVICE_COMPILE__)
    return __uint_as_float((unsigned int)w);
#elif defined(__INTEL_COMPILER)
    return _castu32_f32(w);
#else
    return std::bit_cast<float>(w);
#endif
}

// https://github.com/pytorch/pytorch/blob/main/torch/headeronly/util/floating_point_utils.h
inline uint32_t fp32_to_bits(float f)
{
#if defined(__OPENCL_VERSION__)
    return as_uint(f);
#elif defined(__CUDA_ARCH__) || defined(__HIP_DEVICE_COMPILE__)
    return (uint32_t)__float_as_uint(f);
#elif defined(__INTEL_COMPILER)
    return _castf32_u32(f);
#else
    return std::bit_cast<uint32_t>(f);
#endif
}

template <typename T>
inline constexpr auto Epsilon = std::numeric_limits<T>::epsilon();

// https://entity-toolkit.github.io/wiki/content/useful/float-comparison/#the-simpler-way
template <std::floating_point T>
[[nodiscard]] constexpr bool AlmostEqual(T a, T b, T eps = Epsilon<T>) noexcept
{
    return (a == b) || (std::fabs(a - b) <= std::min(std::fabs(a), std::fabs(b)) * eps);
}

// https://entity-toolkit.github.io/wiki/content/useful/float-comparison/#the-simpler-way
template <std::floating_point T>
[[nodiscard]] constexpr bool AlmostZero(T a, T eps = Epsilon<T>) noexcept
{
    return std::fabs(a) <= eps;
}

// Normalize a value to [0, 1] range
template <std::floating_point T>
[[nodiscard]] constexpr float Normalize(T value, T min, T max) noexcept
{
    assert(min < max);
    if (value <= min)
    {
        return T(0);
    }
    else if (value >= max)
    {
        return T(1);
    }
    else [[likely]]
    {
        return (value - min) / (max - min);
    }
}

template <std::integral T>
[[nodiscard]] constexpr T QuantizeUnorm(float value, float min, float max) noexcept
{
    float normalized = Normalize(value, min, max);
    float scaled = normalized * float(std::numeric_limits<T>::max());
    return static_cast<T>(std::nearbyintf(scaled));
}

[[nodiscard]] constexpr uint8_t QuantizeUnorm8(float value, float min, float max) noexcept
{
    return QuantizeUnorm<uint8_t>(value, min, max);
}

[[nodiscard]] constexpr uint16_t QuantizeUnorm16(float value, float min, float max) noexcept
{
    return QuantizeUnorm<uint16_t>(value, min, max);
}

template <std::unsigned_integral T>
[[nodiscard]] constexpr float DequantizeUnorm(T value) noexcept
{
    constexpr float interval = 1.0f / float(std::numeric_limits<T>::max());
    return float(value) * interval;
}

[[nodiscard]] constexpr float DequantizeUnorm8(uint8_t value) noexcept
{
    return DequantizeUnorm(value);
}

[[nodiscard]] constexpr float DequantizeUnorm16(uint16_t value) noexcept
{
    return DequantizeUnorm(value);
}

} // namespace rad
