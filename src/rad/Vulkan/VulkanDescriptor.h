#pragma once

#include <rad/Vulkan/VulkanCommon.h>

namespace rad
{

class VulkanDescriptorPool : public RefCounted<VulkanDescriptorPool>
{
public:
    VulkanDescriptorPool(Ref<VulkanDevice> device, const vk::DescriptorPoolCreateInfo& createInfo);
    ~VulkanDescriptorPool();

    VulkanDevice* GetDevice() const { return m_device.get(); }
    const vk::detail::DispatchLoaderDynamic& GetDispatcher() const;
    const vk::DescriptorPool& GetHandle() const { return m_handle; }

    std::vector<Ref<VulkanDescriptorSet>> Allocate(vk::ArrayProxy<vk::DescriptorSetLayout> layouts);

private:
    Ref<VulkanDevice> m_device;
    vk::DescriptorPool m_handle = nullptr;

}; // class VulkanDescriptorPool

class VulkanDescriptorSetLayout : public RefCounted<VulkanDescriptorSetLayout>
{
public:
    VulkanDescriptorSetLayout(Ref<VulkanDevice> device,
                              const vk::DescriptorSetLayoutCreateInfo& createInfo);
    ~VulkanDescriptorSetLayout();

    VulkanDevice* GetDevice() const { return m_device.get(); }
    const vk::detail::DispatchLoaderDynamic& GetDispatcher() const;
    const vk::DescriptorSetLayout& GetHandle() const { return m_handle; }

private:
    Ref<VulkanDevice> m_device;
    vk::DescriptorSetLayout m_handle = nullptr;

}; // class VulkanDescriptorSetLayout

class VulkanDescriptorSet : public RefCounted<VulkanDescriptorSet>
{
public:
    VulkanDescriptorSet(Ref<VulkanDescriptorPool> descPool, vk::DescriptorSet descSetHandle);
    ~VulkanDescriptorSet();

    VulkanDevice* GetDevice() const { return m_descPool->GetDevice(); }
    const vk::detail::DispatchLoaderDynamic& GetDispatcher() const;
    VulkanDescriptorPool* GetDescriptorPool() const { return m_descPool.get(); }
    const vk::DescriptorSet& GetHandle() const { return m_handle; }

private:
    Ref<VulkanDescriptorPool> m_descPool;
    vk::DescriptorSet m_handle = nullptr;

    void Update(vk::ArrayProxy<vk::WriteDescriptorSet> writes,
                vk::ArrayProxy<vk::CopyDescriptorSet> copies);

}; // class VulkanDescriptorSet

} // namespace rad
