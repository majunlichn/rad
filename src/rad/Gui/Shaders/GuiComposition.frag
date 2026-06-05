#version 450

layout(push_constant) uniform PushConstants
{
    // VkColorSpaceKHR for the active swapchain (see VulkanFrame::GetSwapchainColorSpace).
    uint colorSpace;
    // SDR-referred white (linear 1.0) luminance in cd/m^2 for HDR color spaces.
    float referenceWhiteNits;
} g_constants;

layout(set = 0, binding = 0) uniform sampler2D g_sceneColorImage;
layout(set = 0, binding = 1) uniform sampler2D g_guiColorImage;

layout(location = 0) in vec2 in_texCoord;
layout(location = 0) out vec4 out_color;

const uint kColorSpaceSrgbNonlinear = 0u; // VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
const uint kColorSpaceExtendedSrgbLinear = 1000104002u; // VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT
const uint kColorSpaceHdr10St2084 = 1000104008u; // VK_COLOR_SPACE_HDR10_ST2084_EXT

// scRGB defines linear 1.0 as 80 cd/m^2.
const float kScRgbWhiteNits = 80.0;
// ST2084 (PQ) peak luminance.
const float kPqMaxNits = 10000.0;

vec3 SrgbToLinear(vec3 srgb)
{
    vec3 lo = srgb / 12.92;
    vec3 hi = pow((srgb + 0.055) / 1.055, vec3(2.4));
    return mix(lo, hi, greaterThan(srgb, vec3(0.04045)));
}

vec3 LinearToSrgb(vec3 linear)
{
    vec3 lo = linear * 12.92;
    vec3 hi = 1.055 * pow(linear, vec3(1.0 / 2.4)) - 0.055;
    return mix(lo, hi, greaterThan(linear, vec3(0.0031308)));
}

// Linear Rec.709/sRGB primaries -> linear Rec.2020 primaries (column-major for GLSL).
vec3 Rec709ToRec2020(vec3 c)
{
    const mat3 m = mat3(0.627402, 0.069095, 0.016394,  // column 0
                        0.329292, 0.919544, 0.088028,  // column 1
                        0.043306, 0.011360, 0.895578); // column 2
    return m * c;
}

// ST2084 / PQ OETF. Input is luminance normalized to [0, 1] where 1.0 == 10000 cd/m^2.
vec3 PqOetf(vec3 l)
{
    const float m1 = 2610.0 / 16384.0;
    const float m2 = 2523.0 / 4096.0 * 128.0;
    const float c1 = 3424.0 / 4096.0;
    const float c2 = 2413.0 / 4096.0 * 32.0;
    const float c3 = 2392.0 / 4096.0 * 32.0;
    vec3 lm1 = pow(max(l, vec3(0.0)), vec3(m1));
    return pow((c1 + c2 * lm1) / (1.0 + c3 * lm1), vec3(m2));
}

void main()
{
    vec4 sceneColorImage = texture(g_sceneColorImage, in_texCoord);
    vec4 guiColorImage = texture(g_guiColorImage, in_texCoord);
    // ImGui writes display-referred sRGB into an UNORM target; decode to linear for compositing.
    guiColorImage.rgb = SrgbToLinear(guiColorImage.rgb);

    // Linear, Rec.709-primaries, SDR-referred (white == 1.0) composite.
    vec3 color = mix(sceneColorImage.rgb, guiColorImage.rgb, guiColorImage.a);

    if (g_constants.colorSpace == kColorSpaceHdr10St2084)
    {
        vec3 rec2020 = max(Rec709ToRec2020(color), vec3(0.0));
        vec3 nits = rec2020 * g_constants.referenceWhiteNits;
        out_color = vec4(PqOetf(nits / kPqMaxNits), 1.0);
    }
    else if (g_constants.colorSpace == kColorSpaceExtendedSrgbLinear)
    {
        out_color = vec4(color * (g_constants.referenceWhiteNits / kScRgbWhiteNits), 1.0);
    }
    else // VK_COLOR_SPACE_SRGB_NONLINEAR_KHR and other unhandled spaces
    {
        out_color = vec4(LinearToSrgb(color), 1.0);
    }
}
