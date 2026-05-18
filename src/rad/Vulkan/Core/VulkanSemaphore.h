#pragma once

#include <rad/Vulkan/VulkanCommon.h>

namespace rad
{

class VulkanSemaphore : public RefCounted<VulkanSemaphore>
{
public:
    VulkanSemaphore(Ref<VulkanDevice> device, const vk::SemaphoreCreateInfo& createInfo);
    ~VulkanSemaphore();

    VulkanDevice* GetDevice() const { return m_device.get(); }
    const vk::detail::DispatchLoaderDynamic& GetDispatcher() const;
    const vk::Semaphore& GetHandle() const { return m_handle; }

private:
    Ref<VulkanDevice> m_device;
    vk::Semaphore m_handle = nullptr;

}; // class VulkanSemaphore

} // namespace rad
