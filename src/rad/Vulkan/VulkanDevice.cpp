#include <rad/System/MemoryDebug.h>

#include <rad/Vulkan/VulkanDevice.h>

#include <rad/Vulkan/VulkanInstance.h>

namespace rad
{

VulkanDevice::VulkanDevice(rad::Ref<VulkanInstance> instance,
                           vk::raii::PhysicalDevice physicalDevice,
                           const VulkanDeviceConfig& config) :
    m_instance(std::move(instance)),
    m_physicalDevice(physicalDevice)
{
    m_config = config;

    m_properties = m_physicalDevice.getProperties();

    VulkanVersion apiVersion = m_instance->GetApiVersion();
    if (apiVersion.IsGreaterEqualThan(1, 1, 0))
    {
        auto properties2Chain = &m_properties2.pNext;
        if (apiVersion.IsGreaterEqualThan(1, 2, 0))
        {
            VK_STRUCTURE_CHAIN_LINK(properties2Chain, m_driverProperties);
            VK_STRUCTURE_CHAIN_LINK(properties2Chain, m_vk11Properties);
            VK_STRUCTURE_CHAIN_LINK(properties2Chain, m_vk12Properties);
        }
        if (apiVersion.IsGreaterEqualThan(1, 3, 0))
        {
            VK_STRUCTURE_CHAIN_LINK(properties2Chain, m_vk13Properties);
        }
        m_physicalDevice.getDispatcher()->vkGetPhysicalDeviceProperties2(
            static_cast<vk::PhysicalDevice>(m_physicalDevice),
            reinterpret_cast<VkPhysicalDeviceProperties2*>(&m_properties2));
    }

    m_queueFamilyProperties = m_physicalDevice.getQueueFamilyProperties();
    m_memoryProperties = m_physicalDevice.getMemoryProperties();

    m_features = m_physicalDevice.getFeatures();
    if (apiVersion.IsGreaterEqualThan(1, 1, 0))
    {
        auto features2Chain = &m_features2.pNext;
        if (m_config.enableVulkan11Features && apiVersion.IsGreaterEqualThan(1, 1, 0))
        {
            VK_STRUCTURE_CHAIN_LINK(features2Chain, m_Vulkan11Features);
        }
        if (m_config.enableVulkan12Features && apiVersion.IsGreaterEqualThan(1, 2, 0))
        {
            VK_STRUCTURE_CHAIN_LINK(features2Chain, m_Vulkan12Features);
        }
        if (m_config.enableVulkan13Features && apiVersion.IsGreaterEqualThan(1, 3, 0))
        {
            VK_STRUCTURE_CHAIN_LINK(features2Chain, m_Vulkan13Features);
        }
        if (m_config.enableVulkan14Features && apiVersion.IsGreaterEqualThan(1, 4, 0))
        {
            VK_STRUCTURE_CHAIN_LINK(features2Chain, m_Vulkan14Features);
        }
        m_physicalDevice.getDispatcher()->vkGetPhysicalDeviceFeatures2(
            static_cast<vk::PhysicalDevice>(m_physicalDevice),
            reinterpret_cast<VkPhysicalDeviceFeatures2*>(&m_features2));
    }

    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
    m_queueFamilyProperties = m_physicalDevice.getQueueFamilyProperties();
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
        }
    }

    const std::vector<vk::ExtensionProperties> supportedExtensions =
        m_physicalDevice.enumerateDeviceExtensionProperties();
    const std::set<std::string>& requiredExtensions = m_config.extensions;
    m_enabledExtensions.clear();
    std::vector<const char*> enabledExtensions;
    for (const std::string extension : requiredExtensions)
    {
        if (HasExtension(supportedExtensions, extension))
        {
            auto [iter, inserted] = m_enabledExtensions.insert(extension);
            if (inserted)
            {
                enabledExtensions.push_back(iter->c_str());
            }
        }
    }

    vk::DeviceCreateInfo createInfo = {};
    createInfo.pNext = &m_features2;
    createInfo.flags = {};
    createInfo.setQueueCreateInfos(queueCreateInfos);
    createInfo.setPEnabledExtensionNames(enabledExtensions);
    createInfo.pEnabledFeatures = nullptr;

    m_handle = m_physicalDevice.createDevice(createInfo);

    // Vma Initialization
    // https://gpuopen-librariesandsdks.github.io/VulkanMemoryAllocator/html/quick_start.html#quick_start_initialization
    VmaAllocatorCreateInfo allocatorCreateInfo = {};
    allocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_3;
    allocatorCreateInfo.instance = m_instance->GetHandle();
    allocatorCreateInfo.physicalDevice = static_cast<vk::PhysicalDevice>(m_physicalDevice);
    allocatorCreateInfo.device = static_cast<vk::Device>(m_handle);
    VmaVulkanFunctions vmaFunctions = {};
    vmaFunctions.vkGetInstanceProcAddr = m_physicalDevice.getDispatcher()->vkGetInstanceProcAddr;
    vmaFunctions.vkGetDeviceProcAddr = m_physicalDevice.getDispatcher()->vkGetDeviceProcAddr;
    allocatorCreateInfo.pVulkanFunctions = &vmaFunctions;
    if (m_Vulkan12Features.bufferDeviceAddress)
    {
        allocatorCreateInfo.flags |= VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
    }
    VK_CHECK_RETURN(vmaCreateAllocator(&allocatorCreateInfo, &m_allocator));
}

VulkanDevice::~VulkanDevice()
{
    if (m_allocator)
    {
        vmaDestroyAllocator(m_allocator);
        m_allocator = VK_NULL_HANDLE;
    }
}

vk::SurfaceCapabilitiesKHR VulkanDevice::GetCapabilities(vk::SurfaceKHR surface) const
{
    return m_physicalDevice.getSurfaceCapabilitiesKHR(surface);
}

std::vector<vk::SurfaceFormatKHR> VulkanDevice::GetSurfaceFormats(vk::SurfaceKHR surface) const
{
    return m_physicalDevice.getSurfaceFormatsKHR(surface);
}

std::vector<vk::PresentModeKHR> VulkanDevice::GetPresentModes(vk::SurfaceKHR surface) const
{
    return m_physicalDevice.getSurfacePresentModesKHR(surface);
}

} // namespace rad
