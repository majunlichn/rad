#pragma once

#include <rad/Vulkan/VulkanCommon.h>

namespace rad
{

struct VulkanInstanceConfig
{
    bool enableValidationLayer;
    std::set<std::string> layers;
    std::set<std::string> extensions;
    vk::PFN_DebugUtilsMessengerCallbackEXT pfnUserCallback = VulkanDebugUtilsMessengerCallback;
}; // struct VulkanInstanceConfig

class VulkanInstance : public RefCounted<VulkanInstance>
{
public:
    static Ref<VulkanInstance> Create(cstring_view appName, uint32_t appVersion,
                                      cstring_view engineName, uint32_t engineVersion,
                                      const VulkanInstanceConfig& config = {});

    VulkanInstance();
    ~VulkanInstance();

    const vk::Instance& GetHandle() const { return m_handle; }
    const vk::detail::DispatchLoaderDynamic& GetDispatcher() const { return m_dispatcher; }

    PFN_vkVoidFunction GetProcAddr(const char* name) const
    {
        return m_handle.getProcAddr(name, m_dispatcher);
    }

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

    std::vector<vk::PhysicalDevice>& GetPhysicalDevices() { return m_physicalDevices; }

    Ref<VulkanSurface> CreateSurface(const vk::DisplaySurfaceCreateInfoKHR& createInfo);

private:
    vk::detail::DynamicLoader m_loader;
    vk::detail::DispatchLoaderDynamic m_dispatcher;

    vk::Instance m_handle = nullptr;
    VulkanVersion m_apiVersion = 0;
    VulkanInstanceConfig m_config = {};
    std::set<std::string, StringLess> m_enabledLayers;
    std::set<std::string, StringLess> m_enabledExtensions;

    vk::DebugUtilsMessengerEXT m_debugUtilsMessenger = {nullptr};
    std::vector<vk::PhysicalDevice> m_physicalDevices;

}; // class VulkanInstance

} // namespace rad
