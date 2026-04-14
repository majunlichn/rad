#pragma once

#include <rad/Vulkan/VulkanCommon.h>

namespace rad
{

struct VulkanInstanceConfig
{
    bool enableValidationLayer;
    std::set<std::string> layers;
    std::set<std::string> extensions;
    vk::PFN_DebugUtilsMessengerCallbackEXT pfnUserCallback = DebugUtilsMessengerCallback;
}; // struct VulkanInstanceConfig

class VulkanInstance : public RefCounted<VulkanInstance>
{
public:
    static Ref<VulkanInstance> Create(cstring_view appName, uint32_t appVersion,
                                      cstring_view engineName, uint32_t engineVersion,
                                      const VulkanInstanceConfig& config = {});

    VulkanInstance();
    ~VulkanInstance();

    vk::Instance GetHandle() const { return static_cast<vk::Instance>(m_instance); }
    const VulkanInstanceDispatcher* GetDispatcher() const { return m_instance.getDispatcher(); }

    PFN_vkVoidFunction GetProcAddr(const char* name) const { return m_instance.getProcAddr(name); }

    std::vector<vk::LayerProperties> EnumerateInstanceLayers();
    std::vector<vk::ExtensionProperties> EnumerateInstanceExtensions(
        vk::Optional<const std::string> layerName = nullptr);

    bool Init(cstring_view appName, uint32_t appVersion, cstring_view engineName,
              uint32_t engineVersion, const VulkanInstanceConfig& config = {});

    VulkanVersion GetApiVersion() const { return m_apiVersion; }

    bool IsLayerEnabled(std::string_view name) const
    {
        return (m_enabledLayers.find(name) != m_enabledLayers.end());
    }

    bool IsExtensionEnabled(std::string_view name) const
    {
        return (m_enabledExtensions.find(name) != m_enabledExtensions.end());
    }

    std::vector<vk::raii::PhysicalDevice>& GetPhysicalDevices() { return m_physicalDevices; }

    vk::raii::Context m_apiContext;

    vk::raii::Instance m_instance = {nullptr};
    VulkanVersion m_apiVersion = 0;
    VulkanInstanceConfig m_config = {};
    std::set<std::string, StringLess> m_enabledLayers;
    std::set<std::string, StringLess> m_enabledExtensions;

    vk::raii::DebugUtilsMessengerEXT m_debugUtilsMessenger = {nullptr};
    std::vector<vk::raii::PhysicalDevice> m_physicalDevices;

}; // class VulkanInstance

} // namespace rad
