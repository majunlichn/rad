#include <rad/Common/MemoryDebug.h>

#include <rad/Vulkan/VulkanShader.h>

#include <cstdlib>
#include <cstring>

namespace rad
{

namespace
{

constexpr const char* VulkanShaderRootEnvName = "RAD_VULKAN_SHADER_ROOT";
constexpr const char* DefaultVulkanShaderRoot = "Shaders";

} // namespace

FilePath GetVulkanShaderRoot()
{
    if (const char* envRoot = std::getenv(VulkanShaderRootEnvName))
    {
        if (envRoot[0] != '\0')
        {
            return FilePath(envRoot);
        }
    }
    return FilePath(DefaultVulkanShaderRoot);
}

std::vector<uint32_t> LoadShaderSpv(cstring_view fileName)
{
    const FilePath path = GetVulkanShaderRoot() / fileName.c_str();
    const std::vector<uint8_t> bytes = File::ReadAll(path.string());
    if (bytes.empty() || (bytes.size() % sizeof(uint32_t)) != 0)
    {
        return {};
    }

    std::vector<uint32_t> spv(bytes.size() / sizeof(uint32_t));
    std::memcpy(spv.data(), bytes.data(), bytes.size());
    return spv;
}

} // namespace rad
