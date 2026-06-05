#pragma once

#include <rad/Vulkan/VulkanCommon.h>

#include <array>
#include <cmath>

namespace rad
{

// Display-referred (sRGB) <-> linear conversions for the GUI rendering pipeline.
// All offscreen color attachments are linear UNORM; only the final composite pass encodes for display.

inline float SrgbToLinearChannel(float channel)
{
    return channel <= 0.04045f ? channel / 12.92f
                               : std::pow((channel + 0.055f) / 1.055f, 2.4f);
}

inline float LinearToSrgbChannel(float channel)
{
    return channel <= 0.0031308f ? channel * 12.92f
                                 : 1.055f * std::pow(channel, 1.0f / 2.4f) - 0.055f;
}

inline std::array<float, 4> LinearColorFromSrgb(float r, float g, float b, float a)
{
    return {SrgbToLinearChannel(r), SrgbToLinearChannel(g), SrgbToLinearChannel(b), a};
}

inline vk::ClearColorValue LinearClearFromSrgb(float r, float g, float b, float a)
{
    const std::array<float, 4> linear = LinearColorFromSrgb(r, g, b, a);
    vk::ClearColorValue clear = {};
    clear.setFloat32(linear);
    return clear;
}

} // namespace rad
