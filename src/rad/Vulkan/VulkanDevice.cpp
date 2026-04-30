#include <rad/Common/MemoryDebug.h>

#include <rad/Vulkan/VulkanDevice.h>

#include <rad/Vulkan/VulkanInstance.h>
#include <rad/Vulkan/VulkanQuery.h>
#include <rad/Vulkan/VulkanCommand.h>
#include <rad/Vulkan/VulkanFence.h>
#include <rad/Vulkan/VulkanSemaphore.h>
#include <rad/Vulkan/VulkanEvent.h>
#include <rad/Vulkan/VulkanFramebuffer.h>
#include <rad/Vulkan/VulkanRenderPass.h>
#include <rad/Vulkan/VulkanDescriptor.h>
#include <rad/Vulkan/VulkanPipeline.h>
#include <rad/Vulkan/VulkanBuffer.h>
#include <rad/Vulkan/VulkanImage.h>
#include <rad/Vulkan/VulkanSampler.h>
#include <rad/Vulkan/VulkanSurface.h>
#include <rad/Vulkan/VulkanSwapchain.h>
#include <rad/Vulkan/VulkanQuery.h>

namespace rad
{

VulkanDevice::VulkanDevice(Ref<VulkanInstance> instance, vk::PhysicalDevice physicalDevice,
                           const VulkanDeviceConfig& config) :
    m_instance(std::move(instance)),
    m_physicalDevice(physicalDevice)
{
    m_config = config;

    m_properties = m_physicalDevice.getProperties(m_instance->GetDispatcher());

    VulkanVersion apiVersion = m_instance->GetApiVersion();
    if (apiVersion.IsGreaterEqualThan(1, 1, 0))
    {
        VulkanStructureChain<vk::PhysicalDeviceProperties2> properties2Chain(m_properties2);
        if (apiVersion.IsGreaterEqualThan(1, 2, 0))
        {
            properties2Chain.Link(m_driverProperties);
            properties2Chain.Link(m_vk11Properties);
            properties2Chain.Link(m_vk12Properties);
        }
        if (apiVersion.IsGreaterEqualThan(1, 3, 0))
        {
            properties2Chain.Link(m_vk13Properties);
        }
        m_physicalDevice.getProperties2(&properties2Chain, m_instance->GetDispatcher());
    }

    m_queueFamilyProperties =
        m_physicalDevice.getQueueFamilyProperties(m_instance->GetDispatcher());
    m_memoryProperties = m_physicalDevice.getMemoryProperties(m_instance->GetDispatcher());

    m_features = m_physicalDevice.getFeatures(m_instance->GetDispatcher());
    if (apiVersion.IsGreaterEqualThan(1, 1, 0))
    {
        VulkanStructureChain<vk::PhysicalDeviceFeatures2> features2Chain(m_features2);
        if (m_config.enableVulkan11Features && apiVersion.IsGreaterEqualThan(1, 1, 0))
        {
            features2Chain.Link(m_Vulkan11Features);
        }
        if (m_config.enableVulkan12Features && apiVersion.IsGreaterEqualThan(1, 2, 0))
        {
            features2Chain.Link(m_Vulkan12Features);
        }
        if (m_config.enableVulkan13Features && apiVersion.IsGreaterEqualThan(1, 3, 0))
        {
            features2Chain.Link(m_Vulkan13Features);
        }
        if (m_config.enableVulkan14Features && apiVersion.IsGreaterEqualThan(1, 4, 0))
        {
            features2Chain.Link(m_Vulkan14Features);
        }
        m_physicalDevice.getFeatures2(&features2Chain, m_instance->GetDispatcher());
    }

    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
    m_queueFamilyIndices.fill(VK_QUEUE_FAMILY_IGNORED);
    float priority = 1.0f;
    // Find the universal queue that support both graphics and compute.
    for (uint32_t i = 0; i < m_queueFamilyProperties.size(); i++)
    {
        if ((m_queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics) &&
            (m_queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eCompute))
        {
            vk::DeviceQueueCreateInfo queueInfo = {};
            queueInfo.flags = {};
            queueInfo.queueFamilyIndex = i;
            queueInfo.queueCount = 1;
            queueInfo.pQueuePriorities = &priority;
            queueCreateInfos.emplace_back(queueInfo);
            m_queueFamilyIndices[size_t(VulkanQueueFamily::Universal)] = i;
            break;
        }
    }
    // No queue support both graphics and compute, pick the first compute queue as the universal queue.
    if (!HasQueueFamily(VulkanQueueFamily::Universal))
    {
        for (uint32_t i = 0; i < m_queueFamilyProperties.size(); i++)
        {
            if (m_queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eCompute)
            {
                vk::DeviceQueueCreateInfo queueInfo = {};
                queueInfo.flags = {};
                queueInfo.queueFamilyIndex = i;
                queueInfo.queueCount = 1;
                queueInfo.pQueuePriorities = &priority;
                queueCreateInfos.emplace_back(queueInfo);
                m_queueFamilyIndices[size_t(VulkanQueueFamily::Universal)] = i;
                break;
            }
        }
    }
    assert(HasQueueFamily(VulkanQueueFamily::Universal));
    // Find an async compute queue (ACE) that only support compute:
    for (uint32_t i = 0; i < m_queueFamilyProperties.size(); i++)
    {
        // Async Compute Engine (ACE): different from the universal queue and only support compute.
        if (!(m_queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics) &&
            (m_queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eCompute) &&
            (i != m_queueFamilyIndices[size_t(VulkanQueueFamily::Universal)]))
        {
            vk::DeviceQueueCreateInfo queueInfo = {};
            queueInfo.flags = {};
            queueInfo.queueFamilyIndex = i;
            queueInfo.queueCount = 1;
            queueInfo.pQueuePriorities = &priority;
            queueCreateInfos.emplace_back(queueInfo);
            m_queueFamilyIndices[size_t(VulkanQueueFamily::Compute)] = i;
            break;
        }
    }
    // Find a transfer queue that only support transfer:
    for (uint32_t i = 0; i < m_queueFamilyProperties.size(); i++)
    {
        if (!(m_queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics) &&
            !(m_queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eCompute) &&
            (m_queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eTransfer))
        {
            vk::DeviceQueueCreateInfo queueInfo = {};
            queueInfo.flags = {};
            queueInfo.queueFamilyIndex = i;
            queueInfo.queueCount = 1;
            queueInfo.pQueuePriorities = &priority;
            queueCreateInfos.emplace_back(queueInfo);
            m_queueFamilyIndices[size_t(VulkanQueueFamily::Transfer)] = i;
            break;
        }
    }

    const std::vector<vk::ExtensionProperties> supportedExtensions =
        m_physicalDevice.enumerateDeviceExtensionProperties(nullptr, m_instance->GetDispatcher());
    const std::set<std::string>& requiredExtensions = m_config.extensions;
    m_enabledExtensions.clear();
    std::vector<const char*> enabledExtensions;
    for (const std::string& extension : requiredExtensions)
    {
        if (HasExtension(supportedExtensions, extension))
        {
            auto [iter, inserted] = m_enabledExtensions.insert(extension);
            if (inserted)
            {
                enabledExtensions.push_back(iter->c_str());
            }
        }
        else
        {
            VK_LOG(err, "Required device extension not supported: {}", extension);
            assert(false);
        }
    }

    vk::DeviceCreateInfo createInfo = {};
    createInfo.flags = {};
    createInfo.setQueueCreateInfos(queueCreateInfos);
    createInfo.setPEnabledExtensionNames(enabledExtensions);
    if (apiVersion.IsGreaterEqualThan(1, 1, 0))
    {
        createInfo.pNext = &m_features2;
        createInfo.pEnabledFeatures = nullptr;
    }
    else
    {
        createInfo.pNext = nullptr;
        createInfo.pEnabledFeatures = &m_features;
    }

    m_handle = m_physicalDevice.createDevice(createInfo, nullptr, m_instance->GetDispatcher());
    // Seed device dispatcher from instance dispatcher (vkGetInstanceProcAddr already initialized).
    m_dispatcher = m_instance->GetDispatcher();
    m_dispatcher.init(m_handle);

    // Vma Initialization
    // https://gpuopen-librariesandsdks.github.io/VulkanMemoryAllocator/html/quick_start.html#quick_start_initialization
    VmaAllocatorCreateInfo allocatorCreateInfo = {};
    allocatorCreateInfo.vulkanApiVersion = m_properties.apiVersion;
    vk::Instance instanceHandle = m_instance->GetHandle();
    allocatorCreateInfo.instance = instanceHandle;
    allocatorCreateInfo.physicalDevice = m_physicalDevice;
    allocatorCreateInfo.device = m_handle;
    VmaVulkanFunctions vmaFunctions = {};
    vmaFunctions.vkGetInstanceProcAddr = m_instance->GetDispatcher().vkGetInstanceProcAddr;
    vmaFunctions.vkGetDeviceProcAddr = m_instance->GetDispatcher().vkGetDeviceProcAddr;
    allocatorCreateInfo.pVulkanFunctions = &vmaFunctions;
    if (m_Vulkan12Features.bufferDeviceAddress)
    {
        allocatorCreateInfo.flags |= VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
    }
    VK_CHECK(vmaCreateAllocator(&allocatorCreateInfo, &m_allocator));
}

VulkanDevice::~VulkanDevice()
{
    if (m_handle)
    {
        if (m_allocator)
        {
            vmaDestroyAllocator(m_allocator);
            m_allocator = VK_NULL_HANDLE;
        }
        m_handle.destroy(nullptr, m_dispatcher);
        m_handle = nullptr;
    }
}

vk::SurfaceCapabilitiesKHR VulkanDevice::GetCapabilities(vk::SurfaceKHR surface) const
{
    return m_physicalDevice.getSurfaceCapabilitiesKHR(surface, m_instance->GetDispatcher());
}

std::vector<vk::SurfaceFormatKHR> VulkanDevice::GetSurfaceFormats(vk::SurfaceKHR surface) const
{
    return m_physicalDevice.getSurfaceFormatsKHR(surface, m_instance->GetDispatcher());
}

std::vector<vk::PresentModeKHR> VulkanDevice::GetPresentModes(vk::SurfaceKHR surface) const
{
    return m_physicalDevice.getSurfacePresentModesKHR(surface, m_instance->GetDispatcher());
}

void VulkanDevice::WaitIdle()
{
    m_handle.waitIdle(m_dispatcher);
}

Ref<VulkanCommandPool> VulkanDevice::CreateCommandPool(vk::CommandPoolCreateFlags flags,
                                                       VulkanQueueFamily queueFamily)
{
    vk::CommandPoolCreateInfo createInfo = {};
    createInfo.flags = flags;
    createInfo.queueFamilyIndex = GetQueueFamilyIndex(queueFamily);
    return RAD_NEW VulkanCommandPool(this, queueFamily, createInfo);
}

Ref<VulkanFence> VulkanDevice::CreateFence(const vk::FenceCreateInfo& createInfo)
{
    return RAD_NEW VulkanFence(this, createInfo);
}

Ref<VulkanSemaphore> VulkanDevice::CreateSemaphore(const vk::SemaphoreCreateInfo& createInfo)
{
    return RAD_NEW VulkanSemaphore(this, createInfo);
}

Ref<VulkanEvent> VulkanDevice::CreateEvent(const vk::EventCreateInfo& createInfo)
{
    return RAD_NEW VulkanEvent(this, createInfo);
}

Ref<VulkanFramebuffer> VulkanDevice::CreateFramebuffer(const vk::FramebufferCreateInfo& createInfo)
{
    return RAD_NEW VulkanFramebuffer(this, createInfo);
}

Ref<VulkanRenderPass> VulkanDevice::CreateRenderPass(const vk::RenderPassCreateInfo& createInfo)
{
    return RAD_NEW VulkanRenderPass(this, createInfo);
}

Ref<VulkanDescriptorPool> VulkanDevice::CreateDescriptorPool(
    const vk::DescriptorPoolCreateInfo& createInfo)
{
    return RAD_NEW VulkanDescriptorPool(this, createInfo);
}

Ref<VulkanDescriptorSetLayout> VulkanDevice::CreateDescriptorSetLayout(
    const vk::DescriptorSetLayoutCreateInfo& createInfo)
{
    return RAD_NEW VulkanDescriptorSetLayout(this, createInfo);
}

Ref<VulkanPipelineLayout> VulkanDevice::CreatePipelineLayout(
    const vk::PipelineLayoutCreateInfo& createInfo)
{
    return RAD_NEW VulkanPipelineLayout(this, createInfo);
}

Ref<VulkanShaderModule> VulkanDevice::CreateShaderModule(
    const vk::ShaderModuleCreateInfo& createInfo)
{
    return RAD_NEW VulkanShaderModule(this, createInfo);
}

Ref<VulkanGraphicsPipeline> VulkanDevice::CreateGraphicsPipeline(
    const VulkanGraphicsPipelineCreateInfo& createInfo)
{
    return RAD_NEW VulkanGraphicsPipeline(this, createInfo);
}

Ref<VulkanComputePipeline> VulkanDevice::CreateComputePipeline(
    const VulkanComputePipelineCreateInfo& createInfo)
{
    return RAD_NEW VulkanComputePipeline(this, createInfo);
}

Ref<VulkanBuffer> VulkanDevice::CreateBuffer(const vk::BufferCreateInfo& createInfo,
                                             const VmaAllocationCreateInfo& allocCreateInfo)
{
    return RAD_NEW VulkanBuffer(this, createInfo, allocCreateInfo);
}

Ref<VulkanBuffer> VulkanDevice::CreateBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage,
                                             VmaMemoryUsage memoryUsage,
                                             VmaAllocationCreateFlags allocationFlags)
{
    vk::BufferCreateInfo bufferInfo = {};
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    VmaAllocationCreateInfo allocationInfo = {};
    allocationInfo.usage = memoryUsage;
    allocationInfo.flags = allocationFlags;
    return CreateBuffer(bufferInfo, allocationInfo);
}

Ref<VulkanBuffer> VulkanDevice::CreateUniformBuffer(vk::DeviceSize size, bool persistentMapped)
{
    vk::BufferCreateInfo bufferInfo = {};
    bufferInfo.size = size;
    bufferInfo.usage = vk::BufferUsageFlagBits::eUniformBuffer;
    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
    allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
    if (persistentMapped)
    {
        allocInfo.flags |= VMA_ALLOCATION_CREATE_MAPPED_BIT;
    }
    return CreateBuffer(bufferInfo, allocInfo);
}

Ref<VulkanBuffer> VulkanDevice::CreateStorageBuffer(vk::DeviceSize size,
                                                    vk::BufferUsageFlags extraUsage)
{
    vk::BufferCreateInfo bufferInfo = {};
    bufferInfo.size = size;
    bufferInfo.usage = vk::BufferUsageFlagBits::eStorageBuffer | extraUsage;
    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
    return CreateBuffer(bufferInfo, allocInfo);
}

Ref<VulkanBuffer> VulkanDevice::CreateIndexBuffer(vk::DeviceSize size,
                                                  vk::BufferUsageFlags extraUsage)
{
    vk::BufferCreateInfo bufferInfo = {};
    bufferInfo.size = size;
    bufferInfo.usage = vk::BufferUsageFlagBits::eIndexBuffer | extraUsage;
    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
    return CreateBuffer(bufferInfo, allocInfo);
}

Ref<VulkanBuffer> VulkanDevice::CreateVertexBuffer(vk::DeviceSize size,
                                                   vk::BufferUsageFlags extraUsage)
{
    vk::BufferCreateInfo bufferInfo = {};
    bufferInfo.size = size;
    bufferInfo.usage = vk::BufferUsageFlagBits::eVertexBuffer | extraUsage;
    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
    return CreateBuffer(bufferInfo, allocInfo);
}

Ref<VulkanBuffer> VulkanDevice::CreateStagingBuffer(vk::DeviceSize size,
                                                    vk::BufferUsageFlags extraUsage)
{
    vk::BufferCreateInfo bufferInfo = {};
    bufferInfo.size = size;
    bufferInfo.usage = vk::BufferUsageFlagBits::eTransferSrc | extraUsage;
    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
    allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
    return CreateBuffer(bufferInfo, allocInfo);
}

Ref<VulkanImage> VulkanDevice::CreateImage(const vk::ImageCreateInfo& createInfo,
                                           const VmaAllocationCreateInfo& allocCreateInfo)
{
    return RAD_NEW VulkanImage(this, createInfo, allocCreateInfo);
}

Ref<VulkanImage> VulkanDevice::CreateImage2D(uint32_t width, uint32_t height, vk::Format format,
                                             vk::ImageUsageFlags usage, uint32_t mipLevels,
                                             uint32_t arrayLayers, vk::SampleCountFlagBits samples,
                                             vk::ImageTiling tiling, vk::ImageCreateFlags flags,
                                             VmaMemoryUsage memoryUsage)
{
    vk::ImageCreateInfo imageInfo = {};
    imageInfo.flags = flags;
    imageInfo.imageType = vk::ImageType::e2D;
    imageInfo.format = format;
    imageInfo.extent = vk::Extent3D(width, height, 1);
    imageInfo.mipLevels = mipLevels;
    imageInfo.arrayLayers = arrayLayers;
    imageInfo.samples = samples;
    imageInfo.tiling = tiling;
    imageInfo.usage = usage;
    imageInfo.sharingMode = vk::SharingMode::eExclusive;
    imageInfo.initialLayout = vk::ImageLayout::eUndefined;

    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.usage = memoryUsage;
    return CreateImage(imageInfo, allocInfo);
}

Ref<VulkanSampler> VulkanDevice::CreateSampler(const vk::SamplerCreateInfo& createInfo)
{
    return RAD_NEW VulkanSampler(this, createInfo);
}

Ref<VulkanSwapchain> VulkanDevice::CreateSwapchain(rad::Ref<VulkanSurface> surface,
                                                   const vk::SwapchainCreateInfoKHR& createInfo)
{
    return RAD_NEW VulkanSwapchain(this, std::move(surface), createInfo);
}

Ref<VulkanQueryPool> VulkanDevice::CreateQueryPool(const vk::QueryPoolCreateInfo& createInfo)
{
    return RAD_NEW VulkanQueryPool(this, createInfo);
}

void VulkanDevice::UpdateDescriptorSets(vk::ArrayProxy<vk::WriteDescriptorSet> writes,
                                        vk::ArrayProxy<vk::CopyDescriptorSet> copies)
{
    m_handle.updateDescriptorSets(writes, copies, m_dispatcher);
}

void VulkanDevice::ResetQueryPool(VulkanQueryPool* queryPool, uint32_t firstQuery,
                                  uint32_t queryCount)
{
    m_handle.resetQueryPool(queryPool->GetHandle(), firstQuery, queryCount, m_dispatcher);
}

vk::Result VulkanDevice::GetQueryPoolResults(VulkanQueryPool* queryPool, uint32_t firstQuery,
                                             uint32_t queryCount, size_t dataSize, void* pData,
                                             vk::DeviceSize stride, vk::QueryResultFlags flags)
{
    return m_handle.getQueryPoolResults(queryPool->GetHandle(), firstQuery, queryCount, dataSize,
                                        pData, stride, flags, m_dispatcher);
}

} // namespace rad
