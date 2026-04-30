#include <rad/Vulkan/VulkanSurface.h>

#include <rad/Vulkan/VulkanInstance.h>

namespace rad
{

VulkanSurface::VulkanSurface(Ref<VulkanInstance> instance,
                             const vk::DisplaySurfaceCreateInfoKHR& createInfo) :
    m_instance(std::move(instance))
{
    m_handle =
        m_instance->GetHandle().createDisplayPlaneSurfaceKHR(createInfo, nullptr, GetDispatcher());
}

VulkanSurface::~VulkanSurface()
{
    if (m_handle)
    {
        m_instance->GetHandle().destroySurfaceKHR(m_handle, nullptr, GetDispatcher());
        m_handle = nullptr;
    }
}

const vk::detail::DispatchLoaderDynamic& VulkanSurface::GetDispatcher() const
{
    return m_instance->GetDispatcher();
}

} // namespace rad
