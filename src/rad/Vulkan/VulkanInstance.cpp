#include <rad/Common/MemoryDebug.h>

#include <rad/Vulkan/VulkanInstance.h>

#include <rad/Vulkan/VulkanSurface.h>

#if defined(_WIN32)
#include <Windows.h>
#include <vulkan/vulkan_win32.h>
#endif

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace rad
{

Ref<VulkanInstance> VulkanInstance::Create(cstring_view appName, uint32_t appVersion,
                                           cstring_view engineName, uint32_t engineVersion,
                                           const VulkanInstanceConfig& config)
{
    Ref<VulkanInstance> instance = RAD_NEW VulkanInstance();
    if (!instance->Init(appName, appVersion, engineName, engineVersion, config))
    {
        return nullptr;
    }
    return instance;
}

VulkanInstance::VulkanInstance()
{
    auto vkGetInstanceProcAddr =
        m_loader.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
    m_dispatcher.init(vkGetInstanceProcAddr);
}

VulkanInstance::~VulkanInstance()
{
    if (m_handle)
    {
        if (m_debugUtilsMessenger)
        {
            m_handle.destroyDebugUtilsMessengerEXT(m_debugUtilsMessenger, nullptr, m_dispatcher);
            m_debugUtilsMessenger = nullptr;
        }
        m_handle.destroy(nullptr, m_dispatcher);
        m_handle = nullptr;
    }
}

std::vector<vk::LayerProperties> VulkanInstance::EnumerateInstanceLayers()
{
    return vk::enumerateInstanceLayerProperties(m_dispatcher);
}

std::vector<vk::ExtensionProperties> VulkanInstance::EnumerateInstanceExtensions(
    vk::Optional<const std::string> layerName)
{
    return vk::enumerateInstanceExtensionProperties(layerName, m_dispatcher);
}

bool VulkanInstance::Init(cstring_view appName, uint32_t appVersion, cstring_view engineName,
                          uint32_t engineVersion, const VulkanInstanceConfig& config)
{
    m_apiVersion = vk::enumerateInstanceVersion(m_dispatcher);
    VK_LOG(info, "Instance Version: {}.{}.{}", m_apiVersion.GetMajor(), m_apiVersion.GetMinor(),
           m_apiVersion.GetPatch());

    m_config = config;

    vk::ApplicationInfo appInfo;
    appInfo.pApplicationName = appName.c_str();
    appInfo.applicationVersion = appVersion;
    appInfo.pEngineName = engineName.c_str();
    appInfo.engineVersion = engineVersion;
    appInfo.apiVersion = m_apiVersion;

    vk::InstanceCreateInfo instanceCreateInfo = {};
    VulkanStructureChain<vk::InstanceCreateInfo> instanceCreateInfoChain(instanceCreateInfo);
    instanceCreateInfo.pApplicationInfo = &appInfo;

    auto supportedLayers = EnumerateInstanceLayers();
    auto supportedExtensions = EnumerateInstanceExtensions(nullptr);

    const auto& requestedLayers = m_config.layers;
    const auto& requestedExtensions = m_config.extensions;

    for (const std::string& requestedLayer : requestedLayers)
    {
        if (HasLayer(supportedLayers, requestedLayer))
        {
            m_enabledLayers.insert(requestedLayer);
        }
        else
        {
            VK_LOG(warn, "Requested instance layer not supported: {}", requestedLayer);
        }
    }

    for (const std::string& requestedExtension : requestedExtensions)
    {
        if (HasExtension(supportedExtensions, requestedExtension))
        {
            m_enabledExtensions.insert(requestedExtension);
        }
        else
        {
            VK_LOG(warn, "Requested instance extension not supported: {}", requestedExtension);
        }
    }

    // Layers/extensions that should be enabled by default:
    if (HasExtension(supportedExtensions, VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME))
    {
        m_enabledExtensions.insert(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
    }
    if (HasExtension(supportedExtensions, VK_KHR_SURFACE_EXTENSION_NAME))
    {
        m_enabledExtensions.insert(VK_KHR_SURFACE_EXTENSION_NAME);
    }
    if (HasExtension(supportedExtensions, VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME))
    {
        m_enabledExtensions.insert(VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME);
    }
#if defined(_WIN32)
    if (HasExtension(supportedExtensions, VK_KHR_WIN32_SURFACE_EXTENSION_NAME))
    {
        m_enabledExtensions.insert(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
    }
#endif

    bool enableValidation = m_config.enableValidationLayer;

    if (const char* envVulkanSDKPath = std::getenv("VULKAN_SDK"))
    {
        VK_LOG(info, "VulkanSDK Path: {}", envVulkanSDKPath);
    }
    else
    {
        VK_LOG(warn, "VulkanSDK is not available!");
    }

    if (const char* envEnableValidation = std::getenv("VK_ENABLE_VALIDATION"))
    {
        VK_LOG(info, "VK_ENABLE_VALIDATION={}.", envEnableValidation);
        enableValidation = StrToBool(envEnableValidation);
    }

    if (enableValidation)
    {
        if (HasLayer(supportedLayers, "VK_LAYER_KHRONOS_validation") &&
            HasExtension(supportedExtensions, VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
        {
            m_enabledLayers.insert("VK_LAYER_KHRONOS_validation");
            m_enabledExtensions.insert(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }
        else
        {
            VK_LOG(warn, "Cannot enable validation due to missing layer "
                         "VK_LAYER_KHRONOS_validation or extension VK_EXT_debug_utils!");
        }
    }

    vk::DebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo;
    debugUtilsMessengerCreateInfo.messageSeverity =
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;
    debugUtilsMessengerCreateInfo.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                                                vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
                                                vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;
    debugUtilsMessengerCreateInfo.pfnUserCallback = m_config.pfnUserCallback;

    if (m_enabledLayers.contains("VK_LAYER_KHRONOS_validation") &&
        m_enabledExtensions.contains(VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
    {
        instanceCreateInfoChain.Link(debugUtilsMessengerCreateInfo);
        enableValidation = true;
    }
    else
    {
        enableValidation = false;
    }

    std::vector<const char*> enabledLayers;
    for (const std::string& layer : m_enabledLayers)
    {
        enabledLayers.push_back(layer.c_str());
    }
    instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(enabledLayers.size());
    instanceCreateInfo.ppEnabledLayerNames = enabledLayers.data();

    std::vector<const char*> enabledExtensions;
    for (const std::string& extension : m_enabledExtensions)
    {
        enabledExtensions.push_back(extension.c_str());
    }
    instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(enabledExtensions.size());
    instanceCreateInfo.ppEnabledExtensionNames = enabledExtensions.data();

    VK_LOG(info, "CreateInstance: {}", instanceCreateInfoChain.ToString());
    m_handle = vk::createInstance(instanceCreateInfoChain, nullptr, m_dispatcher);
    m_dispatcher.init(m_handle);
    for (const std::string& layer : m_enabledLayers)
    {
        VK_LOG(info, "Instance layer enabled: {}", layer);
    }
    for (const std::string& extension : m_enabledExtensions)
    {
        VK_LOG(info, "Instance extension enabled: {}", extension);
    }

    if (enableValidation)
    {
        m_debugUtilsMessenger = m_handle.createDebugUtilsMessengerEXT(debugUtilsMessengerCreateInfo,
                                                                      nullptr, m_dispatcher);
    }

    m_physicalDevices = m_handle.enumeratePhysicalDevices(m_dispatcher);
    for (size_t physicalDeviceIndex = 0; physicalDeviceIndex < m_physicalDevices.size();
         physicalDeviceIndex++)
    {
        auto& physicalDevice = m_physicalDevices[physicalDeviceIndex];
        VK_LOG(info, "GPU#{}: {}", physicalDeviceIndex,
               physicalDevice.getProperties(m_dispatcher).deviceName.data());
        auto queueFamilies = physicalDevice.getQueueFamilyProperties(m_dispatcher);
        const uint32_t& apiVersion = physicalDevice.getProperties(m_dispatcher).apiVersion;
        VK_LOG(info, "API Version: {}.{}.{}", VK_VERSION_MAJOR(apiVersion),
               VK_VERSION_MINOR(apiVersion), VK_VERSION_PATCH(apiVersion));
        for (size_t queueFamilyIndex = 0; queueFamilyIndex < queueFamilies.size();
             ++queueFamilyIndex)
        {
            const auto& queueFamily = queueFamilies[queueFamilyIndex];
            VK_LOG(info, "QueueFamily#{}: {}", queueFamilyIndex,
                   vk::to_string(queueFamily.queueFlags));
        }
    }

    return !m_physicalDevices.empty();
}

Ref<VulkanSurface> VulkanInstance::CreateSurface(const vk::DisplaySurfaceCreateInfoKHR& createInfo)
{
    return RAD_NEW VulkanSurface(this, createInfo);
}

} // namespace rad
