#pragma once

#include <rad/Vulkan/VulkanCommon.h>

namespace rad
{

class VulkanBuffer : public RefCounted<VulkanBuffer>
{
public:
    VulkanBuffer(Ref<VulkanDevice> device, const vk::BufferCreateInfo& bufferInfo,
                 const VmaAllocationCreateInfo& allocCreateInfo);
    ~VulkanBuffer();

    static Ref<VulkanBuffer> Create(Ref<VulkanDevice> device, vk::DeviceSize size,
                                    vk::BufferUsageFlags usage,
                                    VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_AUTO,
                                    VmaAllocationCreateFlags allocFlags = 0);

    VulkanDevice* GetDevice() const { return m_device.get(); }
    const vk::detail::DispatchLoaderDynamic& GetDispatcher() const;
    vk::Buffer GetHandle() const { return m_handle; }

    vk::DeviceSize GetSize() const { return m_size; }

    bool IsHostVisible() const
    {
        return bool(m_memPropFlags & vk::MemoryPropertyFlagBits::eHostVisible);
    }
    bool IsHostCoherent() const
    {
        return bool(m_memPropFlags & vk::MemoryPropertyFlagBits::eHostCoherent);
    }

    void* MapMemory();
    void UnmapMemory();

    Ref<VulkanBufferView> CreateView(vk::Format format, vk::DeviceSize offset = 0,
                                     vk::DeviceSize range = vk::WholeSize,
                                     vk::BufferViewCreateFlags flags = {});

private:
    Ref<VulkanDevice> m_device;
    vk::Buffer m_handle = nullptr;
    vk::DeviceSize m_size = 0;
    vk::BufferUsageFlags m_usage;
    VmaAllocation m_alloc = nullptr;
    VmaAllocationInfo m_allocInfo = {};
    vk::MemoryPropertyFlags m_memPropFlags;

}; // class VulkanBuffer

class VulkanBufferView : public RefCounted<VulkanBufferView>
{
public:
    VulkanBufferView(Ref<VulkanBuffer> buffer, const vk::BufferViewCreateInfo& createInfo);
    ~VulkanBufferView();

    VulkanDevice* GetDevice() const { return m_buffer->GetDevice(); }
    const vk::detail::DispatchLoaderDynamic& GetDispatcher() const;
    VulkanBuffer* GetBuffer() const { return m_buffer.get(); }
    vk::BufferView GetHandle() const { return m_handle; }

private:
    Ref<VulkanBuffer> m_buffer;
    vk::BufferView m_handle = nullptr;
    vk::Format m_format = vk::Format::eUndefined;
    vk::DeviceSize m_offset = 0;
    vk::DeviceSize m_range = 0;

}; // class VulkanBufferView

} // namespace rad
