#pragma once

#include <rad/Vulkan/VulkanCommon.h>

namespace rad
{

class VulkanSwapchain : public RefCounted<VulkanSwapchain>
{
public:
    VulkanSwapchain(Ref<VulkanDevice> device, Ref<VulkanSurface> surface,
                    const vk::SwapchainCreateInfoKHR& createInfo);
    ~VulkanSwapchain();

    VulkanDevice* GetDevice() const { return m_device.get(); }
    const vk::detail::DispatchLoaderDynamic& GetDispatcher() const;
    vk::SwapchainKHR GetHandle() const { return m_handle; }

    uint32_t GetImageCount() const { return m_imageCount; }
    vk::Format GetImageFormat() const { return m_imageFormat; }
    vk::ColorSpaceKHR GetImageColorSpace() const { return m_imageColorSpace; }
    vk::Extent2D GetImageExtent() const { return m_imageExtent; }
    uint32_t GetImageWidth() const { return m_imageExtent.width; }
    uint32_t GetImageHeight() const { return m_imageExtent.height; }
    uint32_t GetImageArrayLayers() const { return m_imageArrayLayers; }
    vk::ImageUsageFlags GetImageUsage() const { return m_imageUsage; }
    vk::SurfaceTransformFlagBitsKHR GetPreTransform() const { return m_preTransform; }
    vk::CompositeAlphaFlagBitsKHR GetCompositeAlpha() const { return m_compositeAlpha; }
    vk::PresentModeKHR GetPresentMode() const { return m_presentMode; }

    VulkanImage* GetImage(uint32_t index) const { return m_images[index].get(); }
    VulkanImageView* GetImageView(uint32_t index) const { return m_imageViews[index].get(); }

    vk::Result AcquireNextImage(uint64_t timeout, VulkanSemaphore* semaphore, VulkanFence* fence,
                                uint32_t deviceMask);

    uint32_t GetCurrentImageIndex() const { return m_currentImageIndex; }

private:
    Ref<VulkanDevice> m_device;
    Ref<VulkanSurface> m_surface;
    vk::SwapchainKHR m_handle = nullptr;

    uint32_t m_imageCount = 0;
    vk::Format m_imageFormat = vk::Format::eUndefined;
    vk::ColorSpaceKHR m_imageColorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;
    vk::Extent2D m_imageExtent = {};
    uint32_t m_imageArrayLayers = {};
    vk::ImageUsageFlags m_imageUsage = {};
    vk::SurfaceTransformFlagBitsKHR m_preTransform = vk::SurfaceTransformFlagBitsKHR::eIdentity;
    vk::CompositeAlphaFlagBitsKHR m_compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    vk::PresentModeKHR m_presentMode = vk::PresentModeKHR::eImmediate;

    std::vector<Ref<VulkanImage>> m_images;
    std::vector<Ref<VulkanImageView>> m_imageViews;

    uint32_t m_currentImageIndex = 0;

}; // class VulkanSwapchain

} // namespace rad
