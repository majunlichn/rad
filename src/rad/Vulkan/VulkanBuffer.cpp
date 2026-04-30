#include <rad/Common/MemoryDebug.h>

#include <rad/Vulkan/VulkanBuffer.h>

#include <rad/Vulkan/VulkanDevice.h>

namespace rad
{

VulkanBuffer::VulkanBuffer(Ref<VulkanDevice> device, const vk::BufferCreateInfo& bufferInfo,
                           const VmaAllocationCreateInfo& allocCreateInfo) :
    m_device(std::move(device))
{
    static_assert(sizeof(vk::Buffer) == sizeof(VkBuffer));
    static_assert(sizeof(vk::BufferCreateInfo) == sizeof(VkBufferCreateInfo));
    VK_CHECK(vmaCreateBuffer(
        m_device->GetAllocator(), reinterpret_cast<const VkBufferCreateInfo*>(&bufferInfo),
        &allocCreateInfo, reinterpret_cast<VkBuffer*>(&m_handle), &m_alloc, &m_allocInfo));
    if (m_handle && m_alloc)
    {
        m_size = bufferInfo.size;
        m_usage = bufferInfo.usage;
        vmaGetAllocationMemoryProperties(m_device->GetAllocator(), m_alloc,
                                         reinterpret_cast<VkMemoryPropertyFlags*>(&m_memPropFlags));
    }
}

VulkanBuffer::~VulkanBuffer()
{
    if (m_handle && m_alloc)
    {
        vmaDestroyBuffer(m_device->GetAllocator(), m_handle, m_alloc);
        m_handle = nullptr;
        m_alloc = nullptr;
    }
}

Ref<VulkanBuffer> VulkanBuffer::Create(Ref<VulkanDevice> device, vk::DeviceSize size,
                                       vk::BufferUsageFlags usage, VmaMemoryUsage memoryUsage,
                                       VmaAllocationCreateFlags allocFlags)
{
    vk::BufferCreateInfo bufferInfo = {};
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.usage = memoryUsage;
    allocInfo.flags = allocFlags;
    return RAD_NEW VulkanBuffer(std::move(device), bufferInfo, allocInfo);
}

const vk::detail::DispatchLoaderDynamic& VulkanBuffer::GetDispatcher() const
{
    return m_device->GetDispatcher();
}

void* VulkanBuffer::MapMemory()
{
    void* pMappedAddr = nullptr;
    VK_CHECK(vmaMapMemory(m_device->GetAllocator(), m_alloc, &pMappedAddr));
    return pMappedAddr;
}

void VulkanBuffer::UnmapMemory()
{
    vmaUnmapMemory(m_device->GetAllocator(), m_alloc);
}

Ref<VulkanBufferView> VulkanBuffer::CreateView(vk::Format format, vk::DeviceSize offset,
                                               vk::DeviceSize range,
                                               vk::BufferViewCreateFlags flags)
{
    vk::BufferViewCreateInfo viewInfo = {};
    viewInfo.flags = flags;
    viewInfo.buffer = m_handle;
    viewInfo.format = format;
    viewInfo.offset = offset;
    viewInfo.range = range;
    return RAD_NEW VulkanBufferView(this, viewInfo);
}

VulkanBufferView::VulkanBufferView(Ref<VulkanBuffer> buffer,
                                   const vk::BufferViewCreateInfo& createInfo) :
    m_buffer(std::move(buffer))
{
    static_assert(sizeof(vk::BufferView) == sizeof(VkBufferView));
    static_assert(sizeof(vk::BufferViewCreateInfo) == sizeof(VkBufferViewCreateInfo));
    m_handle = GetDevice()->GetHandle().createBufferView(createInfo, nullptr, GetDispatcher());
    if (m_handle)
    {
        m_format = createInfo.format;
        m_offset = createInfo.offset;
        m_range = createInfo.range;
    }
}

VulkanBufferView::~VulkanBufferView()
{
    if (m_handle)
    {
        GetDevice()->GetHandle().destroyBufferView(m_handle, nullptr, GetDispatcher());
        m_handle = nullptr;
    }
}

const vk::detail::DispatchLoaderDynamic& VulkanBufferView::GetDispatcher() const
{
    return GetDevice()->GetDispatcher();
}

} // namespace rad
