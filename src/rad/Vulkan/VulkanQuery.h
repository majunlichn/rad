#pragma once

#include <rad/Vulkan/VulkanCommon.h>

namespace rad
{

class VulkanQueryPool : public RefCounted<VulkanQueryPool>
{
public:
    VulkanQueryPool(Ref<VulkanDevice> device, const vk::QueryPoolCreateInfo& createInfo);
    ~VulkanQueryPool();

    VulkanDevice* GetDevice() const { return m_device.get(); }
    const vk::detail::DispatchLoaderDynamic& GetDispatcher() const;
    const vk::QueryPool& GetHandle() const { return m_handle; }

    vk::QueryType GetQueryType() const { return m_queryType; }
    uint32_t GetQueryCount() const { return m_queryCount; }

private:
    Ref<VulkanDevice> m_device;
    vk::QueryPool m_handle = {nullptr};

    vk::QueryPoolCreateFlags m_flags = {};
    vk::QueryType m_queryType = vk::QueryType::eOcclusion;
    uint32_t m_queryCount = {};

}; // class VulkanQueryPool

} // namespace rad
