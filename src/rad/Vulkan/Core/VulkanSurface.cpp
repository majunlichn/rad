#include <rad/Common/MemoryDebug.h>

#include <rad/Vulkan/Core/VulkanSurface.h>

#include <rad/Vulkan/Core/VulkanInstance.h>

namespace rad
{

Ref<VulkanSurface> VulkanSurface::Create(Ref<VulkanInstance> instance, vk::SurfaceKHR surface)
{
    return RAD_NEW VulkanSurface(std::move(instance), surface);
}

VulkanSurface::VulkanSurface(Ref<VulkanInstance> instance, vk::SurfaceKHR surface) :
    m_instance(std::move(instance)),
    m_handle(surface)
{
}

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
