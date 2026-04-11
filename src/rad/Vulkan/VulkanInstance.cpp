#include <rad/System/MemoryDebug.h>

#include <rad/Vulkan/VulkanInstance.h>

#if defined(_WIN32)
#include <Windows.h>
#include <vulkan/vulkan_win32.h>
#endif

namespace rad
{

vk::raii::Context VulkanInstance::s_raiiContext;

Ref<VulkanInstance> VulkanInstance::Create(std::string_view appName, uint32_t appVersion,
                                           std::string_view engineName, uint32_t engineVersion,
                                           const VulkanInstanceConfig& config)
{
    Ref<VulkanInstance> instance = RAD_NEW VulkanInstance();
    instance->Init(appName, appVersion, engineName, engineVersion, config);
    return instance;
}

VulkanInstance::VulkanInstance()
{
}

VulkanInstance::~VulkanInstance()
{
}

std::vector<vk::LayerProperties> VulkanInstance::EnumerateInstanceLayers()
{
    return s_raiiContext.enumerateInstanceLayerProperties();
}

std::vector<vk::ExtensionProperties> VulkanInstance::EnumerateInstanceExtensions(
    vk::Optional<const std::string> layerName)
{
    return s_raiiContext.enumerateInstanceExtensionProperties(layerName);
}

bool VulkanInstance::Init(std::string_view appName, uint32_t appVersion,
                          std::string_view engineName, uint32_t engineVersion,
                          const VulkanInstanceConfig& config)
{
    m_apiVersion = s_raiiContext.enumerateInstanceVersion();
    VK_LOG(info, "Instance Version: {}.{}.{}", VK_VERSION_MAJOR(m_apiVersion),
           VK_VERSION_MINOR(m_apiVersion), VK_VERSION_PATCH(m_apiVersion));

    m_config = config;

    vk::ApplicationInfo appInfo;
    appInfo.pApplicationName = appName.data();
    appInfo.applicationVersion = appVersion;
    appInfo.pEngineName = engineName.data();
    appInfo.engineVersion = engineVersion;
    appInfo.apiVersion = m_apiVersion;

    vk::InstanceCreateInfo instanceCreateInfo;
    auto instanceCreateInfoChain = &instanceCreateInfo.pNext;
    instanceCreateInfo.pApplicationInfo = &appInfo;

    auto supportedLayers = EnumerateInstanceLayers();
    auto supportedExtensions = EnumerateInstanceExtensions(nullptr);

    const auto& requiredLayers = m_config.requiredLayers;
    const auto& requiredExtensions = m_config.requiredExtensions;

    for (const std::string& requiredLayer : requiredLayers)
    {
        if (HasLayer(supportedLayers, requiredLayer))
        {
            m_enabledLayers.insert(requiredLayer);
        }
        else
        {
            VK_LOG(warn, "Instance layer not supported: {}", requiredLayer);
        }
    }

    for (const std::string& requiredExtension : requiredExtensions)
    {
        if (HasExtension(supportedExtensions, requiredExtension))
        {
            m_enabledExtensions.insert(requiredExtension);
        }
        else
        {
            VK_LOG(warn, "Instance extension not supported: {}", requiredExtension);
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
    debugUtilsMessengerCreateInfo.pfnUserCallback;

    if (m_enabledLayers.contains("VK_LAYER_KHRONOS_validation") &&
        m_enabledExtensions.contains(VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
    {
        VK_STRUCTURE_CHAIN_LINK(instanceCreateInfoChain, debugUtilsMessengerCreateInfo);
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

    m_instance = s_raiiContext.createInstance(instanceCreateInfo);
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
        m_debugUtilsMessenger =
            m_instance.createDebugUtilsMessengerEXT(debugUtilsMessengerCreateInfo);
    }

    m_physicalDevices = m_instance.enumeratePhysicalDevices();
    for (size_t physicalDeviceIndex = 0; physicalDeviceIndex < m_physicalDevices.size();
         physicalDeviceIndex++)
    {
        auto& physicalDevice = m_physicalDevices[physicalDeviceIndex];
        VK_LOG(info, "GPU#{}: {}", physicalDeviceIndex,
               physicalDevice.getProperties().deviceName.data());
        auto queueFamilies = physicalDevice.getQueueFamilyProperties();
        const uint32_t& apiVersion = physicalDevice.getProperties().apiVersion;
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

} // namespace rad
