#pragma once

#include <rad/Vulkan/VulkanCommon.h>

namespace rad
{

class VulkanSurface : public RefCounted<VulkanSurface>
{
public:
    VulkanSurface(Ref<VulkanInstance> instance, const vk::DisplaySurfaceCreateInfoKHR& createInfo);
    ~VulkanSurface();

    VulkanInstance* GetInstance() const { return m_instance.get(); }
    const vk::detail::DispatchLoaderDynamic& GetDispatcher() const;
    const vk::SurfaceKHR& GetHandle() const { return m_handle; }

private:
    Ref<VulkanInstance> m_instance;
    vk::SurfaceKHR m_handle;

}; // class VulkanSurface

} // namespace rad
