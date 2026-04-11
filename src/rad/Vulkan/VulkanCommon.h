#pragma once

#define VK_NO_PROTOTYPES 1
#define VK_ENABLE_BETA_EXTENSIONS 1

#include <rad/Common/RefCounted.h>
#include <rad/IO/Logging.h>
#include <rad/Container/SmallVector.h>
#include <rad/Container/Span.h>

#include <map>
#include <set>
#include <source_location>

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#define VULKAN_HPP_FLAGS_MASK_TYPE_AS_PUBLIC 1
#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vulkan_enums.hpp>

#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 1
#include <vk_mem_alloc.h>

namespace rad
{

spdlog::logger* GetVulkanLogger();

#define VK_LOG(LogLevel, ...)                                                                      \
    SPDLOG_LOGGER_CALL(rad::GetVulkanLogger(), spdlog::level::LogLevel, __VA_ARGS__)

void ReportError(vk::Result result, const char* call,
                 std::source_location sourceLoc = std::source_location::current());

#define VK_CHECK_RETURN(Call)                                                                      \
    do                                                                                             \
    {                                                                                              \
        const vk::Result result_ = static_cast<vk::Result>(Call);                                  \
        if (result_ != vk::Result::eSuccess)                                                       \
        {                                                                                          \
            ReportError(result_, #Call);                                                           \
        }                                                                                          \
    } while (0)

#define VK_STRUCTURE_CHAIN_LINK(Iter, Next)                                                        \
    do                                                                                             \
    {                                                                                              \
        *Iter = &Next;                                                                             \
        Iter = &Next.pNext;                                                                        \
    } while (0)

struct VulkanVersion
{
    uint32_t m_bits = 0;

    VulkanVersion() = default;
    VulkanVersion(uint32_t bits) :
        m_bits(bits)
    {
    }
    VulkanVersion(uint32_t variant, uint32_t major, uint32_t minor, uint32_t patch) :
        m_bits(VK_MAKE_API_VERSION(variant, major, minor, patch))
    {
    }

    operator uint32_t() const { return m_bits; }

    void Set(uint32_t variant, uint32_t major, uint32_t minor, uint32_t patch)
    {
        m_bits = VK_MAKE_API_VERSION(variant, major, minor, patch);
    }

    uint32_t GetVariant() const { return VK_API_VERSION_VARIANT(m_bits); }
    uint32_t GetMajor() const { return VK_API_VERSION_MAJOR(m_bits); }
    uint32_t GetMinor() const { return VK_API_VERSION_MINOR(m_bits); }
    uint32_t GetPatch() const { return VK_API_VERSION_PATCH(m_bits); }

    bool IsGreater(uint32_t major, uint32_t minor, uint32_t patch) const
    {
        return (VK_API_VERSION_MAJOR(m_bits) > major) || (VK_API_VERSION_MINOR(m_bits) > minor) ||
               (VK_API_VERSION_PATCH(m_bits) > patch);
    }
    bool IsGreaterEqual(uint32_t major, uint32_t minor, uint32_t patch) const
    {
        return (VK_API_VERSION_MAJOR(m_bits) >= major) && (VK_API_VERSION_MINOR(m_bits) >= minor) &&
               (VK_API_VERSION_PATCH(m_bits) >= patch);
    }

}; // struct VulkanVersion

using VulkanInstanceDispatcher = vk::raii::detail::InstanceDispatcher;
using VulkanDeviceDispatcher = vk::raii::detail::DeviceDispatcher;

inline bool HasLayer(const Span<vk::LayerProperties>& layers, std::string_view name)
{
    for (const auto& layer : layers)
    {
        if (StrEqual(layer.layerName, name))
        {
            return true;
        }
    }
    return false;
}

inline bool HasExtension(Span<vk::ExtensionProperties> extensions, std::string_view name)
{
    for (const auto& extension : extensions)
    {
        if (StrEqual(extension.extensionName, name))
        {
            return true;
        }
    }
    return false;
}

VKAPI_ATTR vk::Bool32 VKAPI_CALL DebugUtilsMessengerCallback(
    vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    vk::DebugUtilsMessageTypeFlagsEXT messageTypes,
    const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

} // namespace rad
