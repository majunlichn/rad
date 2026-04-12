#pragma once

#define VK_NO_PROTOTYPES 1
#define VK_ENABLE_BETA_EXTENSIONS 1

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#define VULKAN_HPP_FLAGS_MASK_TYPE_AS_PUBLIC 1
#include <vulkan/vulkan_raii.hpp>

#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 1
#include <vma/vk_mem_alloc.h>

namespace rad
{

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

} // namespace rad
