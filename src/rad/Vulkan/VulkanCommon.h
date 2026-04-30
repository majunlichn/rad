#pragma once

#define VK_NO_PROTOTYPES 1
#define VK_ENABLE_BETA_EXTENSIONS 1

#include <rad/Common/RefCounted.h>
#include <rad/Common/TypeTraits.h>
#include <rad/IO/Logging.h>
#include <rad/Container/SmallVector.h>
#include <rad/Container/Span.h>

#include <map>
#include <set>
#include <source_location>
#include <type_traits>
#include <utility>

#define VULKAN_HPP_NO_DEFAULT_DISPATCHER 1
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>

#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 1
#include <vma/vk_mem_alloc.h>

namespace rad
{

spdlog::logger* GetVulkanLogger();

#define VK_LOG(LogLevel, ...)                                                                      \
    SPDLOG_LOGGER_CALL(::rad::GetVulkanLogger(), spdlog::level::LogLevel, __VA_ARGS__)

void ReportVulkanError(vk::Result result, const char* expr,
                       std::source_location sourceLoc = std::source_location::current());

#define VK_CHECK(Expr)                                                                             \
    do                                                                                             \
    {                                                                                              \
        const vk::Result result_ = static_cast<vk::Result>(Expr);                                  \
        if (result_ != vk::Result::eSuccess)                                                       \
        {                                                                                          \
            ReportVulkanError(result_, #Expr);                                                     \
        }                                                                                          \
    } while (0)

#define VK_STRUCTURE_CHAIN_LINK(Iter, Next)                                                        \
    do                                                                                             \
    {                                                                                              \
        *Iter = &Next;                                                                             \
        Iter = &Next.pNext;                                                                        \
    } while (0)

template <typename T>
concept VulkanStructureConcept = requires(T t) {
    requires std::is_standard_layout_v<T>;
    requires offsetof(T, sType) == 0;
    requires offsetof(T, pNext) == offsetof(vk::BaseInStructure, pNext);
    requires std::is_same_v<decltype(t.sType), VkStructureType> ||
                 std::is_same_v<decltype(t.sType), vk::StructureType>;
    { t.pNext } -> std::convertible_to<const void*>;
};

template <typename T>
concept VulkanBaseOutStructureConcept = requires(T t) {
    requires std::is_standard_layout_v<T>;
    requires offsetof(T, sType) == 0;
    requires offsetof(T, pNext) == offsetof(vk::BaseOutStructure, pNext);
    requires std::is_same_v<decltype(t.sType), VkStructureType> ||
                 std::is_same_v<decltype(t.sType), vk::StructureType>;
    { t.pNext } -> std::convertible_to<void*>;
};

// Simple pNext chain helper.
// Lifetime is owned by the caller; all chained structs must outlive the Vulkan call that consumes
// the chain.
template <VulkanStructureConcept Head>
class VulkanStructureChain
{
public:
    using PNext = decltype(std::declval<Head&>().pNext);
    using Iter = vk::BaseInStructure*;
    using ConstIter = const vk::BaseInStructure*;

    VulkanStructureChain(Head& head) :
        m_head(head)
    {
        Reset();
    }

    operator Head&() { return m_head; }
    operator const Head&() const { return m_head; }
    Head* operator&() { return &m_head; }

    void Reset()
    {
        m_head.pNext = nullptr;
        m_tail = reinterpret_cast<Iter>(&m_head);
    }

    template <VulkanStructureConcept Node>
    void Link(Node& next)
    {
        assert(next.pNext == nullptr);
        auto node = reinterpret_cast<Iter>(&next);
        m_tail->pNext = node;
        m_tail = node;
    }

    std::string ToString() const
    {
        std::string str;
        ConstIter current = reinterpret_cast<ConstIter>(&m_head);
        while (current)
        {
            str += vk::to_string(current->sType) + "->";
            current = reinterpret_cast<ConstIter>(current->pNext);
        }
        str += "Null";
        return str;
    }

private:
    Head& m_head;
    Iter m_tail = nullptr;
}; // class VulkanStructureChain

struct VulkanVersion
{
    uint32_t m_bits = 0;

    VulkanVersion() = default;
    VulkanVersion(uint32_t bits) :
        m_bits(bits)
    {
    }
    VulkanVersion(uint32_t major, uint32_t minor, uint32_t patch) :
        m_bits(VK_MAKE_API_VERSION(0, major, minor, patch))
    {
    }

    operator uint32_t() const { return m_bits; }

    void SetVariant(uint32_t variant)
    {
        m_bits = VK_MAKE_API_VERSION(variant, VK_API_VERSION_MAJOR(m_bits),
                                     VK_API_VERSION_MINOR(m_bits), VK_API_VERSION_PATCH(m_bits));
    }

    uint32_t GetVariant() const { return VK_API_VERSION_VARIANT(m_bits); }
    uint32_t GetMajor() const { return VK_API_VERSION_MAJOR(m_bits); }
    uint32_t GetMinor() const { return VK_API_VERSION_MINOR(m_bits); }
    uint32_t GetPatch() const { return VK_API_VERSION_PATCH(m_bits); }

    bool IsLowerThan(uint32_t major, uint32_t minor, uint32_t patch) const
    {
        if (GetMajor() != major)
        {
            return GetMajor() < major;
        }
        if (GetMinor() != minor)
        {
            return GetMinor() < minor;
        }
        return GetPatch() < patch;
    }

    bool IsLowerEqualThan(uint32_t major, uint32_t minor, uint32_t patch) const
    {
        if (GetMajor() != major)
        {
            return GetMajor() < major;
        }
        if (GetMinor() != minor)
        {
            return GetMinor() < minor;
        }
        return GetPatch() <= patch;
    }

    bool IsGreaterThan(uint32_t major, uint32_t minor, uint32_t patch) const
    {
        if (GetMajor() != major)
        {
            return GetMajor() > major;
        }
        if (GetMinor() != minor)
        {
            return GetMinor() > minor;
        }
        return GetPatch() > patch;
    }

    bool IsGreaterEqualThan(uint32_t major, uint32_t minor, uint32_t patch) const
    {
        if (GetMajor() != major)
        {
            return GetMajor() > major;
        }
        if (GetMinor() != minor)
        {
            return GetMinor() > minor;
        }
        return GetPatch() >= patch;
    }

    bool operator==(const VulkanVersion& other) const { return m_bits == other.m_bits; }
    bool operator!=(const VulkanVersion& other) const { return m_bits != other.m_bits; }
    bool operator<(const VulkanVersion& other) const
    {
        return IsLowerThan(other.GetMajor(), other.GetMinor(), other.GetPatch());
    }
    bool operator<=(const VulkanVersion& other) const
    {
        return IsLowerEqualThan(other.GetMajor(), other.GetMinor(), other.GetPatch());
    }
    bool operator>(const VulkanVersion& other) const
    {
        return IsGreaterThan(other.GetMajor(), other.GetMinor(), other.GetPatch());
    }
    bool operator>=(const VulkanVersion& other) const
    {
        return IsGreaterEqualThan(other.GetMajor(), other.GetMinor(), other.GetPatch());
    }

}; // struct VulkanVersion

inline bool HasLayer(Span<const vk::LayerProperties> layers, cstring_view name)
{
    for (const auto& layer : layers)
    {
        if (StrEqual(layer.layerName.data(), name))
        {
            return true;
        }
    }
    return false;
}

inline bool HasExtension(Span<const vk::ExtensionProperties> extensions, cstring_view name)
{
    for (const auto& extension : extensions)
    {
        if (StrEqual(extension.extensionName.data(), name))
        {
            return true;
        }
    }
    return false;
}

VKAPI_ATTR vk::Bool32 VKAPI_CALL VulkanDebugUtilsMessengerCallback(
    vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    vk::DebugUtilsMessageTypeFlagsEXT messageTypes,
    const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

enum class VulkanQueueFamily : uint32_t
{
    Universal,
    Compute,  // Async Compute Engine (ACE)
    Transfer, // Async Transfer (DMA)
    Count
};

vk::ImageAspectFlags GetDefaultImageAspectFlags(vk::Format format);

// Forward declarations
class VulkanInstance;
class VulkanDevice;
class VulkanCommandPool;
class VulkanCommandBuffer;
class VulkanFence;
class VulkanSemaphore;
class VulkanEvent;
class VulkanFramebuffer;
class VulkanRenderPass;
class VulkanDescriptorPool;
class VulkanDescriptorSetLayout;
class VulkanDescriptorSet;
class VulkanPipelineLayout;
class VulkanShaderModule;
class VulkanPipeline;
class VulkanGraphicsPipelineCreateInfo;
class VulkanComputePipelineCreateInfo;
class VulkanGraphicsPipeline;
class VulkanComputePipeline;
class VulkanBuffer;
class VulkanBufferView;
class VulkanImage;
class VulkanImageView;
class VulkanSampler;
class VulkanSurface;
class VulkanSwapchain;
class VulkanQueryPool;

} // namespace rad
