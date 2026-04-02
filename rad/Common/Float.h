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

[[nodiscard]] float DequantizeUnorm8(uint8_t value) noexcept
{
    return DequantizeUnorm(value);
}

[[nodiscard]] float DequantizeUnorm16(uint16_t value) noexcept
{
    return DequantizeUnorm(value);
}

} // namespace rad
