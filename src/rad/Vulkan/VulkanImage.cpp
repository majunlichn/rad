#include <rad/Common/MemoryDebug.h>

#include <rad/Vulkan/VulkanImage.h>

#include <rad/Vulkan/VulkanDevice.h>

#include <vulkan/utility/vk_format_utils.h>

namespace rad
{

VulkanImage::VulkanImage(Ref<VulkanDevice> device, const vk::ImageCreateInfo& imageInfo,
                         const VmaAllocationCreateInfo& allocCreateInfo) :
    m_device(std::move(device))
{
    VK_CHECK(vmaCreateImage(
        m_device->GetAllocator(), reinterpret_cast<const VkImageCreateInfo*>(&imageInfo),
        &allocCreateInfo, reinterpret_cast<VkImage*>(&m_handle), &m_alloc, &m_allocInfo));
    if (m_handle && m_alloc)
    {
        m_flags = imageInfo.flags;
        m_imageType = imageInfo.imageType;
        m_format = imageInfo.format;
        m_extent = imageInfo.extent;
        m_mipLevels = imageInfo.mipLevels;
        m_arrayLayers = imageInfo.arrayLayers;
        m_samples = imageInfo.samples;
        m_tiling = imageInfo.tiling;
        m_usage = imageInfo.usage;
        m_sharingMode = imageInfo.sharingMode;
        vmaGetAllocationMemoryProperties(m_device->GetAllocator(), m_alloc,
                                         reinterpret_cast<VkMemoryPropertyFlags*>(&m_memPropFlags));
    }
}

VulkanImage::VulkanImage(Ref<VulkanDevice> device, const vk::ImageCreateInfo& imageInfo,
                         vk::Image imageHandle) :
    m_device(std::move(device))
{
    m_handle = imageHandle;

    m_imageType = imageInfo.imageType;
    m_format = imageInfo.format;
    m_extent = imageInfo.extent;
    m_mipLevels = imageInfo.mipLevels;
    m_arrayLayers = imageInfo.arrayLayers;
    m_samples = imageInfo.samples;
    m_tiling = imageInfo.tiling;
    m_usage = imageInfo.usage;
    m_sharingMode = imageInfo.sharingMode;

    m_alloc = nullptr; // not managed
    m_memPropFlags = vk::MemoryPropertyFlagBits::eDeviceLocal;
}

VulkanImage::~VulkanImage()
{
    if (m_handle && m_alloc)
    {
        vmaDestroyImage(m_device->GetAllocator(), m_handle, m_alloc);
        m_handle = nullptr;
        m_alloc = nullptr;
    }
}

const vk::detail::DispatchLoaderDynamic& VulkanImage::GetDispatcher() const
{
    return m_device->GetDispatcher();
}

Ref<VulkanImageView> VulkanImage::CreateView(vk::ImageViewType type, vk::Format format,
                                             const vk::ImageSubresourceRange& range,
                                             vk::ComponentMapping components)
{
    vk::ImageViewCreateInfo viewInfo = {};
    viewInfo.image = m_handle;
    viewInfo.viewType = type;
    viewInfo.format = format;
    viewInfo.components = components;
    viewInfo.subresourceRange = range;
    return RAD_NEW VulkanImageView(this, viewInfo);
}

Ref<VulkanImageView> VulkanImage::CreateView(vk::ImageViewType type, vk::Format format)
{
    vk::ImageSubresourceRange range = {};
    range.aspectMask = GetDefaultImageAspectFlags(m_format);
    range.baseMipLevel = 0;
    range.levelCount = m_mipLevels;
    range.baseArrayLayer = 0;
    range.layerCount = m_arrayLayers;
    return CreateView(type, format, range);
}

Ref<VulkanImageView> VulkanImage::CreateView(vk::ImageViewType type)
{
    return CreateView(type, m_format);
}

Ref<VulkanImageView> VulkanImage::CreateView2D(uint32_t baseMipLevel, uint32_t levelCount,
                                               uint32_t baseArrayLayer)
{
    vk::ImageSubresourceRange range = {};
    range.aspectMask = GetDefaultImageAspectFlags(m_format);
    range.baseMipLevel = baseMipLevel;
    range.levelCount = levelCount;
    range.baseArrayLayer = baseArrayLayer;
    range.layerCount = 1;
    return CreateView(vk::ImageViewType::e2D, m_format, range);
}

VulkanImageView::VulkanImageView(Ref<VulkanImage> image,
                                 const vk::ImageViewCreateInfo& createInfo) :
    m_image(std::move(image))
{
    m_handle = GetDevice()->GetHandle().createImageView(createInfo, nullptr, GetDispatcher());
    if (m_handle)
    {
        m_type = createInfo.viewType;
        m_format = createInfo.format;
        m_range = createInfo.subresourceRange;
        m_components = createInfo.components;
    }
}

VulkanImageView::~VulkanImageView()
{
    if (m_handle)
    {
        GetDevice()->GetHandle().destroyImageView(m_handle, nullptr, GetDispatcher());
        m_handle = nullptr;
    }
}

const vk::detail::DispatchLoaderDynamic& VulkanImageView::GetDispatcher() const
{
    return GetDevice()->GetDispatcher();
}

} // namespace rad
