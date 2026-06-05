#pragma once

#include <rad/Vulkan/VulkanCommon.h>

#include <rad/IO/File.h>

#include <cstdint>
#include <vector>

namespace rad
{

// Root directory for precompiled SPIR-V shaders. Override with the RAD_VULKAN_SHADER_ROOT
// environment variable; when unset or empty, defaults to "Shaders/" (relative to the process cwd).
FilePath GetVulkanShaderRoot();

// Loads SPIR-V words from GetVulkanShaderRoot() / fileName. fileName is relative to that root
// (e.g. "GuiComposition.vert.spv"). Returns an empty vector if the file is missing or not word-aligned.
std::vector<uint32_t> LoadShaderSpv(cstring_view fileName);

} // namespace rad
