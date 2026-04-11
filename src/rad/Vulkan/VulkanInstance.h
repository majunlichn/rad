#pragma once

#include <rad/Vulkan/VulkanCommon.h>

namespace rad
{

struct VulkanInstanceConfig
{
    bool enableValidationLayer;
    std::set<std::string> requiredLayers;
    std::set<std::string> requiredExtensions;
}; // struct VulkanInstanceConfig

class VulkanInstance : public RefCounted<VulkanInstance>
{
public:
    static Ref<VulkanInstance> Create(std::string_view appName, uint32_t appVersion,
                                      std::string_view engineName, uint32_t engineVersion,
                                      const VulkanInstanceConfig& config = {});

    VulkanInstance();
    ~VulkanInstance();

    vk::Instance GetHandle() const { return static_cast<vk::Instance>(m_instance); }
    const VulkanInstanceDispatcher* GetDispatcher() const { return m_instance.getDispatcher(); }

    PFN_vkVoidFunction GetProcAddr(const char* name) const { return m_instance.getProcAddr(name); }

    std::vector<vk::LayerProperties> EnumerateInstanceLayers();
    std::vector<vk::ExtensionProperties> EnumerateInstanceExtensions(
        vk::Optional<const std::string> layerName = nullptr);

    bool Init(std::string_view appName, uint32_t appVersion, std::string_view engineName,
              uint32_t engineVersion, const VulkanInstanceConfig& config = {});

    uint32_t GetApiVersion() const { return m_apiVersion; }

    bool IsLayerEnabled(std::string_view name) const
    {
        return (m_enabledLayers.find(name) != m_enabledLayers.end());
    }

    bool IsExtensionEnabled(std::string_view name) const
    {
        return (m_enabledExtensions.find(name) != m_enabledExtensions.end());
    }

    std::vector<vk::raii::PhysicalDevice>& GetPhysicalDevices() { return m_physicalDevices; }

    static vk::raii::Context s_raiiContext;

    vk::raii::Instance m_instance = {nullptr};
    uint32_t m_apiVersion = 0;
    VulkanInstanceConfig m_config = {};
    std::set<std::string, StringLess> m_enabledLayers;
    std::set<std::string, StringLess> m_enabledExtensions;

    vk::raii::DebugUtilsMessengerEXT m_debugUtilsMessenger = {nullptr};
    std::vector<vk::raii::PhysicalDevice> m_physicalDevices;

}; // class VulkanInstance

} // namespace rad
