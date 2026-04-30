#include <rad/Common/MemoryDebug.h>

#include <rad/Vulkan/VulkanDescriptor.h>
#include <rad/Vulkan/VulkanDevice.h>

namespace rad
{

VulkanDescriptorPool::VulkanDescriptorPool(Ref<VulkanDevice> device,
                                           const vk::DescriptorPoolCreateInfo& createInfo) :
    m_device(std::move(device))
{
    m_handle = m_device->GetHandle().createDescriptorPool(createInfo, nullptr, GetDispatcher());
}

VulkanDescriptorPool::~VulkanDescriptorPool()
{
    if (m_handle)
    {
        m_device->GetHandle().destroyDescriptorPool(m_handle, nullptr, GetDispatcher());
        m_handle = nullptr;
    }
}

const vk::detail::DispatchLoaderDynamic& VulkanDescriptorPool::GetDispatcher() const
{
    return m_device->GetDispatcher();
}

std::vector<Ref<VulkanDescriptorSet>> VulkanDescriptorPool::Allocate(
    vk::ArrayProxy<vk::DescriptorSetLayout> layouts)
{
    vk::DescriptorSetAllocateInfo allocateInfo = {};
    allocateInfo.descriptorPool = m_handle;
    allocateInfo.setSetLayouts(layouts);
    vk::Device deviceHandle = m_device->GetHandle();
    std::vector<vk::DescriptorSet> descSetsHandles(layouts.size());
    VK_CHECK(GetDispatcher().vkAllocateDescriptorSets(
        deviceHandle, reinterpret_cast<const VkDescriptorSetAllocateInfo*>(&allocateInfo),
        reinterpret_cast<VkDescriptorSet*>(descSetsHandles.data())));
    std::vector<Ref<VulkanDescriptorSet>> descSets(descSetsHandles.size());
    for (size_t i = 0; i < descSets.size(); ++i)
    {
        descSets[i] = RAD_NEW VulkanDescriptorSet(this, descSetsHandles[i]);
    }
    return descSets;
}

VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(
    Ref<VulkanDevice> device, const vk::DescriptorSetLayoutCreateInfo& createInfo) :
    m_device(std::move(device))
{
    m_handle =
        m_device->GetHandle().createDescriptorSetLayout(createInfo, nullptr, GetDispatcher());
}

VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout()
{
    if (m_handle)
    {
        m_device->GetHandle().destroyDescriptorSetLayout(m_handle, nullptr, GetDispatcher());
        m_handle = nullptr;
    }
}

const vk::detail::DispatchLoaderDynamic& VulkanDescriptorSetLayout::GetDispatcher() const
{
    return m_device->GetDispatcher();
}

VulkanDescriptorSet::VulkanDescriptorSet(Ref<VulkanDescriptorPool> descPool,
                                         vk::DescriptorSet descSetHandle) :
    m_descPool(std::move(descPool))
{
    m_handle = vk::DescriptorSet(descSetHandle);
}

VulkanDescriptorSet::~VulkanDescriptorSet()
{
    if (m_handle)
    {
        VK_CHECK(GetDevice()->GetHandle().freeDescriptorSets(m_descPool->GetHandle(), 1, &m_handle,
                                                             GetDispatcher()));
        m_handle = nullptr;
    }
}

const vk::detail::DispatchLoaderDynamic& VulkanDescriptorSet::GetDispatcher() const
{
    return GetDevice()->GetDispatcher();
}

void VulkanDescriptorSet::Update(vk::ArrayProxy<vk::WriteDescriptorSet> writes,
                                 vk::ArrayProxy<vk::CopyDescriptorSet> copies)
{
    GetDevice()->GetHandle().updateDescriptorSets(writes, copies, GetDispatcher());
}

} // namespace rad
