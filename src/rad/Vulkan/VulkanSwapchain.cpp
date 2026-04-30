#include <rad/Common/MemoryDebug.h>

#include <rad/Vulkan/VulkanSwapchain.h>

#include <rad/Vulkan/VulkanDevice.h>
#include <rad/Vulkan/VulkanSurface.h>
#include <rad/Vulkan/VulkanImage.h>
#include <rad/Vulkan/VulkanSemaphore.h>
#include <rad/Vulkan/VulkanFence.h>

namespace rad
{

VulkanSwapchain::VulkanSwapchain(Ref<VulkanDevice> device, Ref<VulkanSurface> surface,
                                 const vk::SwapchainCreateInfoKHR& createInfo) :
    m_device(std::move(device)),
    m_surface(std::move(surface))
{
    m_handle = m_device->GetHandle().createSwapchainKHR(createInfo, nullptr, GetDispatcher());
    if (m_handle)
    {
        std::vector<vk::Image> imageHandles;
        VK_CHECK(m_device->GetHandle().getSwapchainImagesKHR(m_handle, &m_imageCount, nullptr,
                                                             GetDispatcher()));
        imageHandles.resize(m_imageCount);
        VK_CHECK(m_device->GetHandle().getSwapchainImagesKHR(m_handle, &m_imageCount,
                                                             imageHandles.data(), GetDispatcher()));
        m_imageCount = static_cast<uint32_t>(imageHandles.size());
        m_imageFormat = createInfo.imageFormat;
        m_imageColorSpace = createInfo.imageColorSpace;
        m_imageExtent = createInfo.imageExtent;
        m_imageArrayLayers = createInfo.imageArrayLayers;
        m_imageUsage = createInfo.imageUsage;
        m_preTransform = createInfo.preTransform;
        m_compositeAlpha = createInfo.compositeAlpha;
        m_presentMode = createInfo.presentMode;

        m_images.resize(imageHandles.size());
        m_imageViews.resize(imageHandles.size());
        for (size_t i = 0; i < imageHandles.size(); ++i)
        {
            vk::ImageCreateInfo imageInfo = {};
            imageInfo.imageType = vk::ImageType::e2D;
            imageInfo.format = createInfo.imageFormat;
            imageInfo.extent = vk::Extent3D(createInfo.imageExtent, 1);
            imageInfo.mipLevels = 1;
            imageInfo.arrayLayers = createInfo.imageArrayLayers;
            imageInfo.samples = vk::SampleCountFlagBits::e1;
            imageInfo.tiling = vk::ImageTiling::eOptimal;
            imageInfo.usage = createInfo.imageUsage;
            imageInfo.sharingMode = createInfo.imageSharingMode;
            imageInfo.queueFamilyIndexCount = createInfo.queueFamilyIndexCount;
            imageInfo.pQueueFamilyIndices = createInfo.pQueueFamilyIndices;
            imageInfo.initialLayout = vk::ImageLayout::eUndefined;
            m_images[i] = RAD_NEW VulkanImage(m_device, imageInfo, imageHandles[i]);
            m_imageViews[i] = m_images[i]->CreateView2D();
        }
    }
}

VulkanSwapchain::~VulkanSwapchain()
{
    // Image views must be destroyed before the swapchain; swapchain images are not application-owned.
    m_imageViews.clear();
    m_images.clear();
    if (m_handle)
    {
        m_device->GetHandle().destroySwapchainKHR(m_handle, nullptr, GetDispatcher());
        m_handle = nullptr;
    }
}

const vk::detail::DispatchLoaderDynamic& VulkanSwapchain::GetDispatcher() const
{
    return m_device->GetDispatcher();
}

vk::Result VulkanSwapchain::AcquireNextImage(uint64_t timeout, VulkanSemaphore* semaphore,
                                             VulkanFence* fence, uint32_t deviceMask)
{
    vk::AcquireNextImageInfoKHR acquireInfo;
    acquireInfo.swapchain = m_handle;
    acquireInfo.timeout = timeout;
    acquireInfo.semaphore = semaphore ? semaphore->GetHandle() : nullptr;
    acquireInfo.fence = fence ? fence->GetHandle() : nullptr;
    acquireInfo.deviceMask = deviceMask;
    auto [result, index] = m_device->GetHandle().acquireNextImage2KHR(acquireInfo, GetDispatcher());
    if ((result == vk::Result::eSuccess) || (result == vk::Result::eSuboptimalKHR))
    {
        m_currentImageIndex = index;
    }
    return result;
}

} // namespace rad
