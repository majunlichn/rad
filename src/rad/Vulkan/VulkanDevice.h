#pragma once

#include <rad/Vulkan/VulkanCommon.h>

namespace rad
{

struct VulkanDeviceConfig
{
    bool enableVulkan11Features;
    bool enableVulkan12Features;
    bool enableVulkan13Features;
    bool enableVulkan14Features;
    std::set<std::string> extensions;
}; // struct VulkanDeviceConfig

class VulkanDevice : public RefCounted<VulkanDevice>
{
public:
    VulkanDevice(rad::Ref<VulkanInstance> instance, vk::raii::PhysicalDevice physicalDevice,
                 const VulkanDeviceConfig& config);
    ~VulkanDevice();

    vk::PhysicalDevice GetPhysicalDevice() const { return m_physicalDevice; }
    vk::Device GetHandle() const { return static_cast<vk::Device>(m_handle); }
    const VulkanDeviceDispatcher* GetDispatcher() const { return m_handle.getDispatcher(); }
    PFN_vkVoidFunction GetProcAddr(const char* name) const { return m_handle.getProcAddr(name); }

    const char* GetName() const { return m_properties.deviceName; }

    vk::SurfaceCapabilitiesKHR GetCapabilities(vk::SurfaceKHR surface) const;
    std::vector<vk::SurfaceFormatKHR> GetSurfaceFormats(vk::SurfaceKHR surface) const;
    std::vector<vk::PresentModeKHR> GetPresentModes(vk::SurfaceKHR surface) const;

    uint32_t GetQueueFamilyIndex(VulkanQueueFamily queueFamily) const
    {
        return m_queueFamilyIndices[size_t(queueFamily)];
    }

    void SetQueueFamilyIndex(VulkanQueueFamily queueFamily, uint32_t index)
    {
        m_queueFamilyIndices[size_t(queueFamily)] = index;
    }

    bool HasQueueFamily(VulkanQueueFamily queueFamily) const
    {
        return (m_queueFamilyIndices[size_t(queueFamily)] != VK_QUEUE_FAMILY_IGNORED);
    }

    const vk::QueueFamilyProperties& GetQueueFamilyProperties(VulkanQueueFamily queueFamily) const
    {
        return m_queueFamilyProperties[GetQueueFamilyIndex(queueFamily)];
    }

    vk::raii::Queue GetQueue(VulkanQueueFamily queueFamily)
    {
        return m_handle.getQueue(GetQueueFamilyIndex(queueFamily), 0);
    }

    vk::raii::Queue GetQueue(const vk::DeviceQueueInfo2& queueInfo)
    {
        return m_handle.getQueue2(queueInfo);
    }

    std::set<std::string, rad::StringLess> m_enabledExtensions;
    bool IsExtensionEnabled(std::string_view name) const
    {
        return m_enabledExtensions.contains(name);
    }

    rad::Ref<VulkanInstance> m_instance;
    vk::raii::PhysicalDevice m_physicalDevice;
    vk::raii::Device m_handle = {nullptr};
    std::array<uint32_t, UnderlyingCast(VulkanQueueFamily::Count)> m_queueFamilyIndices;
    VmaAllocator m_allocator = nullptr;

    VulkanDeviceConfig m_config;

    vk::PhysicalDeviceProperties m_properties;
    vk::PhysicalDeviceProperties2 m_properties2;
    vk::PhysicalDeviceDriverProperties m_driverProperties;
    vk::PhysicalDeviceVulkan11Properties m_vk11Properties;
    vk::PhysicalDeviceVulkan12Properties m_vk12Properties;
    vk::PhysicalDeviceVulkan13Properties m_vk13Properties;

    std::vector<vk::QueueFamilyProperties> m_queueFamilyProperties;
    vk::PhysicalDeviceMemoryProperties m_memoryProperties;

    vk::PhysicalDeviceFeatures m_features;
    vk::PhysicalDeviceFeatures2 m_features2;
    vk::PhysicalDeviceVulkan11Features m_Vulkan11Features;
    vk::PhysicalDeviceVulkan12Features m_Vulkan12Features;
    vk::PhysicalDeviceVulkan13Features m_Vulkan13Features;
    vk::PhysicalDeviceVulkan14Features m_Vulkan14Features;

}; // class VulkanDevice

} // namespace rad
