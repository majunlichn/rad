#pragma once

#include <rad/Vulkan/VulkanCommon.h>
#include <rad/Vulkan/VulkanPipeline.h>

namespace rad
{

struct VulkanDeviceConfig
{
    bool enableVulkan11Features = false;
    bool enableVulkan12Features = false;
    bool enableVulkan13Features = false;
    bool enableVulkan14Features = false;
    std::set<std::string> extensions;
}; // struct VulkanDeviceConfig

class VulkanDevice : public RefCounted<VulkanDevice>
{
public:
    VulkanDevice(Ref<VulkanInstance> instance, vk::PhysicalDevice physicalDevice,
                 const VulkanDeviceConfig& config);
    ~VulkanDevice();

    VulkanInstance* GetInstance() const { return m_instance.get(); }
    vk::PhysicalDevice GetPhysicalDevice() const { return m_physicalDevice; }
    const vk::Device& GetHandle() const { return m_handle; }
    const vk::detail::DispatchLoaderDynamic& GetDispatcher() const { return m_dispatcher; }

    PFN_vkVoidFunction GetProcAddr(const char* name) const
    {
        return m_handle.getProcAddr(name, m_dispatcher);
    }
    VmaAllocator GetAllocator() const { return m_allocator; }

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

    vk::Queue GetQueue(VulkanQueueFamily queueFamily)
    {
        return m_handle.getQueue(GetQueueFamilyIndex(queueFamily), 0, m_dispatcher);
    }

    vk::Queue GetQueue(const vk::DeviceQueueInfo2& queueInfo)
    {
        return m_handle.getQueue2(queueInfo, m_dispatcher);
    }

    bool IsExtensionEnabled(std::string_view name) const
    {
        return m_enabledExtensions.contains(name);
    }

    void WaitIdle();

    Ref<VulkanCommandPool> CreateCommandPool(
        vk::CommandPoolCreateFlags flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
        VulkanQueueFamily queueFamily = VulkanQueueFamily::Universal);
    Ref<VulkanFence> CreateFence(const vk::FenceCreateInfo& createInfo);
    Ref<VulkanSemaphore> CreateSemaphore(const vk::SemaphoreCreateInfo& createInfo);
    Ref<VulkanEvent> CreateEvent(const vk::EventCreateInfo& createInfo);
    Ref<VulkanFramebuffer> CreateFramebuffer(const vk::FramebufferCreateInfo& createInfo);
    Ref<VulkanRenderPass> CreateRenderPass(const vk::RenderPassCreateInfo& createInfo);
    Ref<VulkanDescriptorPool> CreateDescriptorPool(const vk::DescriptorPoolCreateInfo& createInfo);
    Ref<VulkanDescriptorSetLayout> CreateDescriptorSetLayout(
        const vk::DescriptorSetLayoutCreateInfo& createInfo);
    Ref<VulkanPipelineLayout> CreatePipelineLayout(const vk::PipelineLayoutCreateInfo& createInfo);
    Ref<VulkanShaderModule> CreateShaderModule(const vk::ShaderModuleCreateInfo& createInfo);
    Ref<VulkanGraphicsPipeline> CreateGraphicsPipeline(
        const VulkanGraphicsPipelineCreateInfo& createInfo);
    Ref<VulkanComputePipeline> CreateComputePipeline(
        const VulkanComputePipelineCreateInfo& createInfo);

    Ref<VulkanBuffer> CreateBuffer(const vk::BufferCreateInfo& createInfo,
                                   const VmaAllocationCreateInfo& allocCreateInfo);
    Ref<VulkanBuffer> CreateBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage = {},
                                   VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_AUTO,
                                   VmaAllocationCreateFlags allocationFlags = 0);
    Ref<VulkanBuffer> CreateUniformBuffer(vk::DeviceSize size, bool persistentMapped);
    Ref<VulkanBuffer> CreateStorageBuffer(vk::DeviceSize size,
                                          vk::BufferUsageFlags extraUsage = {});
    Ref<VulkanBuffer> CreateIndexBuffer(vk::DeviceSize size, vk::BufferUsageFlags extraUsage = {});
    Ref<VulkanBuffer> CreateVertexBuffer(vk::DeviceSize size, vk::BufferUsageFlags extraUsage = {});
    Ref<VulkanBuffer> CreateStagingBuffer(vk::DeviceSize size,
                                          vk::BufferUsageFlags extraUsage = {});

    Ref<VulkanImage> CreateImage(const vk::ImageCreateInfo& createInfo,
                                 const VmaAllocationCreateInfo& allocCreateInfo);
    Ref<VulkanImage> CreateImage2D(uint32_t width, uint32_t height, vk::Format format,
                                   vk::ImageUsageFlags usage, uint32_t mipLevels = 1,
                                   uint32_t arrayLayers = 1,
                                   vk::SampleCountFlagBits samples = vk::SampleCountFlagBits::e1,
                                   vk::ImageTiling tiling = vk::ImageTiling::eOptimal,
                                   vk::ImageCreateFlags flags = {},
                                   VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_AUTO);
    Ref<VulkanSampler> CreateSampler(const vk::SamplerCreateInfo& createInfo);

    Ref<VulkanSwapchain> CreateSwapchain(rad::Ref<VulkanSurface> surface,
                                         const vk::SwapchainCreateInfoKHR& createInfo);
    Ref<VulkanQueryPool> CreateQueryPool(const vk::QueryPoolCreateInfo& createInfo);

    void UpdateDescriptorSets(vk::ArrayProxy<vk::WriteDescriptorSet> writes,
                              vk::ArrayProxy<vk::CopyDescriptorSet> copies = {});

    void ResetQueryPool(VulkanQueryPool* queryPool, uint32_t firstQuery, uint32_t queryCount);
    vk::Result GetQueryPoolResults(VulkanQueryPool* queryPool, uint32_t firstQuery,
                                   uint32_t queryCount, size_t dataSize, void* pData,
                                   vk::DeviceSize stride, vk::QueryResultFlags flags);

private:
    Ref<VulkanInstance> m_instance;
    vk::PhysicalDevice m_physicalDevice;
    vk::Device m_handle = nullptr;
    vk::detail::DispatchLoaderDynamic m_dispatcher;
    std::array<uint32_t, UnderlyingCast(VulkanQueueFamily::Count)> m_queueFamilyIndices;
    VmaAllocator m_allocator = nullptr;

    VulkanDeviceConfig m_config;

    std::set<std::string, StringLess> m_enabledExtensions;

    vk::PhysicalDeviceProperties m_properties = {};
    vk::PhysicalDeviceProperties2 m_properties2 = {};
    vk::PhysicalDeviceDriverProperties m_driverProperties = {};
    vk::PhysicalDeviceVulkan11Properties m_vk11Properties = {};
    vk::PhysicalDeviceVulkan12Properties m_vk12Properties = {};
    vk::PhysicalDeviceVulkan13Properties m_vk13Properties = {};

    std::vector<vk::QueueFamilyProperties> m_queueFamilyProperties = {};
    vk::PhysicalDeviceMemoryProperties m_memoryProperties = {};

    vk::PhysicalDeviceFeatures m_features = {};
    vk::PhysicalDeviceFeatures2 m_features2 = {};
    vk::PhysicalDeviceVulkan11Features m_Vulkan11Features = {};
    vk::PhysicalDeviceVulkan12Features m_Vulkan12Features = {};
    vk::PhysicalDeviceVulkan13Features m_Vulkan13Features = {};
    vk::PhysicalDeviceVulkan14Features m_Vulkan14Features = {};

}; // class VulkanDevice

} // namespace rad
