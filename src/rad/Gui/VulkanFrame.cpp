#include <rad/Common/MemoryDebug.h>

#include <rad/Gui/VulkanFrame.h>

#include <rad/Gui/VulkanGuiComposition.h>
#include <rad/Gui/VulkanGuiRenderer.h>
#include <rad/Gui/VulkanWindow.h>

#include <algorithm>
#include <array>
#include <functional>
#include <stdexcept>
#include <vector>

namespace rad
{
namespace
{

uint32_t SelectMinImageCount(const vk::SurfaceCapabilitiesKHR& capabilities, uint32_t desired);

// True when minImageCount can be raised to three (some surfaces cap maxImageCount at 2).
bool IsTripleBufferingSupported(VulkanDevice* device, vk::SurfaceKHR surface)
{
    if (!device || surface == VK_NULL_HANDLE)
    {
        return false;
    }
    const vk::SurfaceCapabilitiesKHR capabilities = device->GetCapabilities(surface);
    return SelectMinImageCount(capabilities, 3) >= 3;
}

constexpr size_t kMaxSurfaceFormatPreferences = 8;

// Ordered swapchain (format, colorSpace) preferences, highest priority first.
size_t BuildSurfaceFormatPreferences(bool enableHdr, ColorPrecision precision,
                                     bool includeSdrFallback, vk::SurfaceFormatKHR* out,
                                     size_t outCapacity)
{
    size_t count = 0;
    auto push = [&](vk::Format format, vk::ColorSpaceKHR colorSpace) {
        if (count < outCapacity)
        {
            out[count++] = vk::SurfaceFormatKHR{format, colorSpace};
        }
    };
    auto pushSdr = [&]() {
        const vk::ColorSpaceKHR colorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;
        if (precision == ColorPrecision::High)
        {
            push(vk::Format::eR16G16B16A16Sfloat, colorSpace);
        }
        push(vk::Format::eA2B10G10R10UnormPack32, colorSpace);
        push(vk::Format::eA2R10G10B10UnormPack32, colorSpace);
        push(vk::Format::eB8G8R8A8Unorm, colorSpace);
        push(vk::Format::eR8G8B8A8Unorm, colorSpace);
    };

    if (enableHdr)
    {
        // HDR10 (PQ) first, then scRGB linear.
        push(vk::Format::eA2B10G10R10UnormPack32, vk::ColorSpaceKHR::eHdr10St2084EXT);
        push(vk::Format::eR16G16B16A16Sfloat, vk::ColorSpaceKHR::eExtendedSrgbLinearEXT);
        if (includeSdrFallback)
        {
            pushSdr();
        }
    }
    else
    {
        pushSdr();
    }
    return count;
}

const vk::SurfaceFormatKHR* PickPreferredSurfaceFormat(
    const std::vector<vk::SurfaceFormatKHR>& available, Span<const vk::SurfaceFormatKHR> preferences)
{
    for (const vk::SurfaceFormatKHR& preference : preferences)
    {
        for (const vk::SurfaceFormatKHR& format : available)
        {
            if (format == preference)
            {
                return &format;
            }
        }
    }
    return nullptr;
}

bool IsHdrSupportedOnSurface(const std::vector<vk::SurfaceFormatKHR>& available)
{
    std::array<vk::SurfaceFormatKHR, kMaxSurfaceFormatPreferences> preferences{};
    const size_t preferenceCount =
        BuildSurfaceFormatPreferences(true, ColorPrecision::High, /*includeSdrFallback=*/false,
                                      preferences.data(), preferences.size());
    return PickPreferredSurfaceFormat(available,
                                      Span<const vk::SurfaceFormatKHR>(preferences.data(),
                                                                       preferenceCount)) != nullptr;
}

vk::SurfaceFormatKHR SelectSurfaceFormat(VulkanDevice* device, vk::SurfaceKHR surface, bool enableHdr,
                                       ColorPrecision precision)
{
    std::array<vk::SurfaceFormatKHR, kMaxSurfaceFormatPreferences> preferences{};
    const size_t preferenceCount = BuildSurfaceFormatPreferences(
        enableHdr, precision, /*includeSdrFallback=*/true, preferences.data(), preferences.size());
    const Span<const vk::SurfaceFormatKHR> preferenceSpan(preferences.data(), preferenceCount);

    const std::vector<vk::SurfaceFormatKHR> availableFormats = device->GetSurfaceFormats(surface);
    if (availableFormats.empty() || (availableFormats.size() == 1 &&
                                     availableFormats[0].format == vk::Format::eUndefined))
    {
        return preferences[0];
    }

    if (const vk::SurfaceFormatKHR* picked =
            PickPreferredSurfaceFormat(availableFormats, preferenceSpan))
    {
        return *picked;
    }

    RAD_LOG_GUI(
        warn, "SelectSurfaceFormat: no preferred pair found; using first available ({} / {})",
        vk::to_string(availableFormats[0].format), vk::to_string(availableFormats[0].colorSpace));
    return availableFormats[0];
}

vk::PresentModeKHR SelectPresentMode(VulkanDevice* device, vk::SurfaceKHR surface,
                                     vk::PresentModeKHR requested)
{
    const std::vector<vk::PresentModeKHR> availableModes = device->GetPresentModes(surface);
    for (vk::PresentModeKHR mode : availableModes)
    {
        if (mode == requested)
        {
            return requested;
        }
    }
    return vk::PresentModeKHR::eFifo;
}

vk::Extent2D SelectSwapchainExtent(const vk::SurfaceCapabilitiesKHR& capabilities, int width,
                                   int height)
{
    if (capabilities.currentExtent.width != UINT32_MAX)
    {
        return capabilities.currentExtent;
    }

    vk::Extent2D extent = {
        static_cast<uint32_t>(width),
        static_cast<uint32_t>(height),
    };
    extent.width = std::clamp(extent.width, capabilities.minImageExtent.width,
                              capabilities.maxImageExtent.width);
    extent.height = std::clamp(extent.height, capabilities.minImageExtent.height,
                               capabilities.maxImageExtent.height);
    return extent;
}

vk::CompositeAlphaFlagBitsKHR SelectCompositeAlpha(const vk::SurfaceCapabilitiesKHR& capabilities)
{
    const vk::CompositeAlphaFlagBitsKHR preferences[] = {
        vk::CompositeAlphaFlagBitsKHR::eOpaque,
        vk::CompositeAlphaFlagBitsKHR::ePreMultiplied,
        vk::CompositeAlphaFlagBitsKHR::ePostMultiplied,
        vk::CompositeAlphaFlagBitsKHR::eInherit,
    };
    for (vk::CompositeAlphaFlagBitsKHR preference : preferences)
    {
        if (capabilities.supportedCompositeAlpha & preference)
        {
            return preference;
        }
    }
    return vk::CompositeAlphaFlagBitsKHR::eOpaque;
}

uint32_t SelectMinImageCount(const vk::SurfaceCapabilitiesKHR& capabilities, uint32_t desired)
{
    uint32_t minImageCount = desired;
    if (minImageCount < capabilities.minImageCount)
    {
        minImageCount = capabilities.minImageCount;
    }
    if (capabilities.maxImageCount > 0 && minImageCount > capabilities.maxImageCount)
    {
        minImageCount = capabilities.maxImageCount;
    }
    return minImageCount;
}

bool IsSceneColorAttachmentFormat(vk::PhysicalDevice physicalDevice,
                                  const vk::detail::DispatchLoaderDynamic& dispatch, vk::Format format)
{
    const vk::FormatProperties properties = physicalDevice.getFormatProperties(format, dispatch);
    const vk::FormatFeatureFlags required = vk::FormatFeatureFlagBits::eColorAttachment |
                                            vk::FormatFeatureFlagBits::eSampledImage;
    return (properties.optimalTilingFeatures & required) == required;
}

template <size_t N>
vk::Format PickFirstSceneColorFormat(vk::PhysicalDevice physicalDevice,
                                     const vk::detail::DispatchLoaderDynamic& dispatch,
                                     const vk::Format (&candidates)[N])
{
    for (vk::Format format : candidates)
    {
        if (IsSceneColorAttachmentFormat(physicalDevice, dispatch, format))
        {
            return format;
        }
    }
    return vk::Format::eR8G8B8A8Unorm;
}

// Scene: HDR swapchains use RGBA16F. SDR Low/Medium: 10-bit UNORM, else RGBA8 UNORM.
// SDR High: RGBA16F when supported, else same as Low/Medium.
vk::Format SelectSceneColorFormat(VulkanDevice* device, vk::ColorSpaceKHR swapchainColorSpace,
                                  ColorPrecision precision)
{
    if (VulkanFrame::IsHdrColorSpace(swapchainColorSpace))
    {
        return vk::Format::eR16G16B16A16Sfloat;
    }

    static const vk::Format kSdr10BitOr8[] = {
        vk::Format::eA2B10G10R10UnormPack32,
        vk::Format::eA2R10G10B10UnormPack32,
        vk::Format::eR8G8B8A8Unorm,
    };
    static const vk::Format kSdrHighPreference[] = {
        vk::Format::eR16G16B16A16Sfloat,
        vk::Format::eA2B10G10R10UnormPack32,
        vk::Format::eA2R10G10B10UnormPack32,
        vk::Format::eR8G8B8A8Unorm,
    };

    if (device)
    {
        const vk::PhysicalDevice physicalDevice = device->GetPhysicalDevice();
        const vk::detail::DispatchLoaderDynamic& dispatch = device->GetDispatcher();
        if (precision == ColorPrecision::High)
        {
            return PickFirstSceneColorFormat(physicalDevice, dispatch, kSdrHighPreference);
        }
        return PickFirstSceneColorFormat(physicalDevice, dispatch, kSdr10BitOr8);
    }
    return vk::Format::eR8G8B8A8Unorm;
}

} // namespace

bool VulkanFrame::IsHdrColorSpace(vk::ColorSpaceKHR colorSpace)
{
    return colorSpace == vk::ColorSpaceKHR::eHdr10St2084EXT ||
           colorSpace == vk::ColorSpaceKHR::eExtendedSrgbLinearEXT;
}

uint32_t VulkanFrame::GetFrameLag(BufferingMode mode)
{
    return mode == BufferingMode::Triple ? 3u : 2u;
}

VulkanFrame::VulkanFrame(VulkanWindow* window) : VulkanFrame(window, Settings{}) {}

VulkanFrame::VulkanFrame(VulkanWindow* window, const Settings& settings) :
    m_window(window)
{
    if (!window || !window->GetSurface() || !window->GetVulkanDevice())
    {
        throw std::runtime_error("VulkanFrame requires an initialized VulkanWindow with a surface");
    }

    m_presentMode = settings.presentMode;
    m_enableHdr = settings.enableHdr;
    m_colorPrecision = settings.colorPrecision;
    m_hdrReferenceWhiteNits = settings.hdrReferenceWhiteNits;

    m_buffering = settings.buffering;
    VulkanDevice* device = window->GetVulkanDevice();
    const vk::SurfaceKHR surfaceHandle = window->GetSurface()->GetHandle();
    if (m_buffering == BufferingMode::Triple && !IsTripleBufferingSupported(device, surfaceHandle))
    {
        RAD_LOG_GUI(warn,
                    "Triple buffering is not supported by the surface (max swapchain images < 3); "
                    "using double buffering");
        m_buffering = BufferingMode::Double;
    }
    m_frameLag = GetFrameLag(m_buffering);
    m_desiredSwapchainImageCount = std::max(settings.swapchainImageCount, GetFrameLag(m_buffering));

    if (!CreateSwapchain())
    {
        throw std::runtime_error("VulkanFrame failed to create swapchain");
    }

    // Swapchain-independent frame sync: command pool/buffers, fences, image-available semaphores.
    if (!InitFrameSync())
    {
        Shutdown();
        throw std::runtime_error("VulkanFrame failed to initialize frame sync");
    }

    // Everything tied to the swapchain: color settings, scene render targets, present semaphores.
    if (!CreateSwapchainDependentResources())
    {
        Shutdown();
        throw std::runtime_error("VulkanFrame failed to create swapchain-dependent resources");
    }

    RAD_LOG_GUI(info, "VulkanFrame swapchain: {} / {}",
                vk::to_string(m_swapchainColorFormat), vk::to_string(m_swapchainColorSpace));
    RAD_LOG_GUI(info, "VulkanFrame scene render targets: {}",
                vk::to_string(m_sceneColorFormat));
}

VulkanFrame::~VulkanFrame()
{
    Shutdown();
}

void VulkanFrame::Shutdown()
{
    if (m_window)
    {
        if (VulkanDevice* device = m_window->GetVulkanDevice())
        {
            device->WaitIdle();
        }
    }
    m_guiComposition.reset();
    m_guiRenderer.reset();
    DestroyFrameSync();
    DestroySwapchain();
    m_currentFrame = 0;
    m_frameState = VulkanFrameState::Idle;
    m_recreateSwapchainRequested = false;
}

bool VulkanFrame::CreateSwapchain(vk::SwapchainKHR oldSwapchain)
{
    VulkanDevice* device = m_window->GetVulkanDevice();
    VulkanSurface* surface = m_window->GetSurface();
    if (!device || !surface)
    {
        return false;
    }

    const vk::SurfaceKHR surfaceHandle = surface->GetHandle();
    const vk::SurfaceCapabilitiesKHR capabilities = device->GetCapabilities(surfaceHandle);
    const vk::SurfaceFormatKHR surfaceFormat =
        SelectSurfaceFormat(device, surfaceHandle, m_enableHdr, m_colorPrecision);

    int width = 0;
    int height = 0;
    if (!m_window->GetSizeInPixels(&width, &height))
    {
        RAD_LOG_GUI(err, "VulkanFrame::CreateSwapchain: GetSizeInPixels failed");
        return false;
    }

    const vk::Extent2D extent = SelectSwapchainExtent(capabilities, width, height);
    m_swapchainMinImageCount = SelectMinImageCount(capabilities, m_desiredSwapchainImageCount);
    const vk::PresentModeKHR presentMode = SelectPresentMode(device, surfaceHandle, m_presentMode);

    vk::SwapchainCreateInfoKHR swapchainInfo = {};
    swapchainInfo.surface = surfaceHandle;
    swapchainInfo.minImageCount = m_swapchainMinImageCount;
    swapchainInfo.imageFormat = surfaceFormat.format;
    swapchainInfo.imageColorSpace = surfaceFormat.colorSpace;
    swapchainInfo.imageExtent = extent;
    swapchainInfo.imageArrayLayers = 1;
    swapchainInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
    swapchainInfo.imageSharingMode = vk::SharingMode::eExclusive;
    swapchainInfo.preTransform = capabilities.currentTransform;
    swapchainInfo.compositeAlpha = SelectCompositeAlpha(capabilities);
    swapchainInfo.presentMode = presentMode;
    swapchainInfo.clipped = VK_TRUE;
    swapchainInfo.oldSwapchain = oldSwapchain;

    m_swapchain = device->CreateSwapchain(surface, swapchainInfo);
    if (!m_swapchain)
    {
        RAD_LOG_GUI(err, "VulkanFrame failed to create VulkanSwapchain");
        return false;
    }

    return true;
}

void VulkanFrame::DestroySwapchain()
{
    m_swapchain.reset();
}

bool VulkanFrame::IsBufferingModeSupported(BufferingMode mode) const
{
    if (mode == BufferingMode::Double)
    {
        return true;
    }
    if (!m_window || !m_window->GetSurface() || !m_window->GetVulkanDevice())
    {
        return false;
    }
    return IsTripleBufferingSupported(m_window->GetVulkanDevice(),
                                      m_window->GetSurface()->GetHandle());
}

void VulkanFrame::SetBufferingMode(BufferingMode mode)
{
    if (mode == m_buffering)
    {
        return;
    }
    if (!IsBufferingModeSupported(mode))
    {
        RAD_LOG_GUI(warn, "VulkanFrame::SetBufferingMode: {} buffering is not supported",
                    mode == BufferingMode::Triple ? "triple" : "double");
        return;
    }
    if (IsRecording())
    {
        RAD_LOG_GUI(warn, "VulkanFrame::SetBufferingMode ignored while recording");
        return;
    }

    if (m_window)
    {
        if (VulkanDevice* device = m_window->GetVulkanDevice())
        {
            device->WaitIdle();
        }
    }

    m_buffering = mode;
    m_frameLag = GetFrameLag(m_buffering);
    if (m_desiredSwapchainImageCount < m_frameLag)
    {
        m_desiredSwapchainImageCount = m_frameLag;
    }
    DestroyFrameSync();
    // The render targets are per in-flight frame, so a buffering change requires recreating the
    // swapchain-dependent resources (this bumps the resource generation for dependent passes).
    if (!InitFrameSync() || !CreateSwapchainDependentResources())
    {
        RAD_LOG_GUI(err, "VulkanFrame::SetBufferingMode failed to reinitialize frame sync");
    }
}

void VulkanFrame::SetHdrReferenceWhiteNits(float nits)
{
    if (nits > 0.0f)
    {
        m_hdrReferenceWhiteNits = nits;
    }
}

bool VulkanFrame::SetPresentMode(vk::PresentModeKHR presentMode)
{
    if (presentMode == m_presentMode)
    {
        return false;
    }
    if (IsRecording())
    {
        RAD_LOG_GUI(warn, "VulkanFrame::SetPresentMode ignored while recording");
        return false;
    }
    m_presentMode = presentMode;
    RecreateSwapchain();
    return true;
}

bool VulkanFrame::SetSwapchainImageCount(uint32_t imageCount)
{
    if (imageCount == m_desiredSwapchainImageCount)
    {
        return false;
    }
    if (IsRecording())
    {
        RAD_LOG_GUI(warn, "VulkanFrame::SetSwapchainImageCount ignored while recording");
        return false;
    }
    m_desiredSwapchainImageCount = imageCount;
    RecreateSwapchain();
    return true;
}

bool VulkanFrame::IsHdrAvailable() const
{
    if (!m_window)
    {
        return false;
    }
    VulkanDevice* device = m_window->GetVulkanDevice();
    VulkanSurface* surface = m_window->GetSurface();
    if (!device || !surface)
    {
        return false;
    }
    return IsHdrSupportedOnSurface(device->GetSurfaceFormats(surface->GetHandle()));
}

bool VulkanFrame::SetEnableHdr(bool enableHdr)
{
    if (enableHdr == m_enableHdr)
    {
        return false;
    }
    m_enableHdr = enableHdr;
    RequestRecreateSwapchain();
    return true;
}

bool VulkanFrame::SetColorPrecision(ColorPrecision precision)
{
    if (precision == m_colorPrecision)
    {
        return false;
    }
    m_colorPrecision = precision;
    RequestRecreateSwapchain();
    return true;
}

void VulkanFrame::RequestRecreateSwapchain()
{
    if (IsRecording())
    {
        m_recreateSwapchainRequested = true;
        return;
    }
    RecreateSwapchain();
}

uint32_t VulkanFrame::GetFrameIndex() const
{
    return RecordingFrameIndex();
}

VulkanCommandBuffer* VulkanFrame::GetCommandBuffer() const
{
    if (!IsRecording())
    {
        return nullptr;
    }
    const uint32_t frameIndex = RecordingFrameIndex();
    return frameIndex < m_framesInFlight.size() ? m_framesInFlight[frameIndex].commandBuffer.get()
                                                : nullptr;
}

VulkanImage* VulkanFrame::GetSceneColorImage() const
{
    return IsRecording() ? GetSceneColorImage(RecordingFrameIndex()) : nullptr;
}

VulkanImageView* VulkanFrame::GetSceneColorImageView() const
{
    return IsRecording() ? GetSceneColorImageView(RecordingFrameIndex()) : nullptr;
}

VulkanImage* VulkanFrame::GetSceneColorImage(uint32_t frameIndex) const
{
    return frameIndex < m_framesInFlight.size()
               ? m_framesInFlight[frameIndex].sceneColorImage.get()
               : nullptr;
}

VulkanImageView* VulkanFrame::GetSceneColorImageView(uint32_t frameIndex) const
{
    return frameIndex < m_framesInFlight.size()
               ? m_framesInFlight[frameIndex].sceneColorImageView.get()
               : nullptr;
}

VulkanImage* VulkanFrame::GetGuiColorImage() const
{
    return IsRecording() ? GetGuiColorImage(RecordingFrameIndex()) : nullptr;
}

VulkanImageView* VulkanFrame::GetGuiColorImageView() const
{
    return IsRecording() ? GetGuiColorImageView(RecordingFrameIndex()) : nullptr;
}

VulkanImage* VulkanFrame::GetGuiColorImage(uint32_t frameIndex) const
{
    return frameIndex < m_framesInFlight.size() ? m_framesInFlight[frameIndex].guiColorImage.get()
                                                : nullptr;
}

VulkanImageView* VulkanFrame::GetGuiColorImageView(uint32_t frameIndex) const
{
    return frameIndex < m_framesInFlight.size()
               ? m_framesInFlight[frameIndex].guiColorImageView.get()
               : nullptr;
}

VulkanImage* VulkanFrame::GetSwapchainImage() const
{
    if (!IsRecording() || !m_swapchain)
    {
        return nullptr;
    }
    return m_swapchain->GetImage(m_swapchain->GetCurrentImageIndex());
}

VulkanImageView* VulkanFrame::GetSwapchainImageView() const
{
    if (!IsRecording() || !m_swapchain)
    {
        return nullptr;
    }
    return m_swapchain->GetImageView(m_swapchain->GetCurrentImageIndex());
}

bool VulkanFrame::InitFrameSync()
{
    if (!m_window)
    {
        return false;
    }

    VulkanDevice* device = m_window->GetVulkanDevice();
    if (!device)
    {
        return false;
    }

    m_commandPool = device->CreateCommandPool();
    if (!m_commandPool)
    {
        return false;
    }

    std::vector<Ref<VulkanCommandBuffer>> commandBuffers =
        m_commandPool->AllocateCommandBuffers(vk::CommandBufferLevel::ePrimary, m_frameLag);
    if (commandBuffers.size() != m_frameLag)
    {
        return false;
    }

    m_framesInFlight.resize(m_frameLag);
    vk::SemaphoreCreateInfo semaphoreInfo;
    vk::FenceCreateInfo fenceInfo;
    fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;

    for (uint32_t i = 0; i < m_frameLag; ++i)
    {
        m_framesInFlight[i].commandBuffer = commandBuffers[i];
        m_framesInFlight[i].imageAvailableSemaphore = device->CreateSemaphore(semaphoreInfo);
        m_framesInFlight[i].fence = device->CreateFence(fenceInfo);
        if (!m_framesInFlight[i].imageAvailableSemaphore || !m_framesInFlight[i].fence)
        {
            return false;
        }
    }

    return true;
}

void VulkanFrame::DestroyFrameSync()
{
    for (FrameResources& frame : m_framesInFlight)
    {
        frame.guiColorImageView.reset();
        frame.guiColorImage.reset();
        frame.sceneColorImageView.reset();
        frame.sceneColorImage.reset();
        frame.fence.reset();
        frame.imageAvailableSemaphore.reset();
        frame.commandBuffer.reset();
    }
    m_framesInFlight.clear();
    m_presentCompleteSemaphores.clear();
    m_commandPool.reset();
    m_currentFrame = 0;
}

bool VulkanFrame::CreateRenderTargets()
{
    if (!m_window)
    {
        return false;
    }

    VulkanDevice* device = m_window->GetVulkanDevice();
    if (!device)
    {
        return false;
    }

    const vk::ImageUsageFlags usage =
        vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled;
    const vk::Format guiColorFormat = GetGuiColorFormat();

    for (FrameResources& frame : m_framesInFlight)
    {
        frame.sceneColorImage =
            device->CreateImage2D(m_extent.width, m_extent.height, m_sceneColorFormat, usage);
        if (!frame.sceneColorImage)
        {
            return false;
        }
        frame.sceneColorImageView = frame.sceneColorImage->CreateView2D();
        if (!frame.sceneColorImageView)
        {
            return false;
        }
        frame.sceneColorImage->SetCurrentLayout(vk::ImageLayout::eUndefined);

        frame.guiColorImage =
            device->CreateImage2D(m_extent.width, m_extent.height, guiColorFormat, usage);
        if (!frame.guiColorImage)
        {
            return false;
        }
        frame.guiColorImageView = frame.guiColorImage->CreateView2D();
        if (!frame.guiColorImageView)
        {
            return false;
        }
        frame.guiColorImage->SetCurrentLayout(vk::ImageLayout::eUndefined);
    }
    return true;
}

bool VulkanFrame::CreatePresentCompleteSemaphores()
{
    if (!m_window)
    {
        return false;
    }

    VulkanDevice* device = m_window->GetVulkanDevice();
    VulkanSwapchain* swapchain = m_swapchain.get();
    if (!device || !swapchain)
    {
        return false;
    }

    m_presentCompleteSemaphores.clear();
    m_presentCompleteSemaphores.resize(swapchain->GetImageCount());

    vk::SemaphoreCreateInfo semaphoreInfo;
    for (Ref<VulkanSemaphore>& semaphore : m_presentCompleteSemaphores)
    {
        semaphore = device->CreateSemaphore(semaphoreInfo);
        if (!semaphore)
        {
            m_presentCompleteSemaphores.clear();
            return false;
        }
    }
    return true;
}

bool VulkanFrame::CreateSwapchainDependentResources()
{
    if (!m_swapchain)
    {
        return false;
    }

    m_extent = m_swapchain->GetImageExtent();
    m_swapchainColorFormat = m_swapchain->GetImageFormat();
    m_swapchainColorSpace = m_swapchain->GetImageColorSpace();
    m_sceneColorFormat = SelectSceneColorFormat(m_window->GetVulkanDevice(), m_swapchainColorSpace,
                                                m_colorPrecision);

    if (!CreateRenderTargets() || !CreatePresentCompleteSemaphores())
    {
        return false;
    }

    return RebuildGui();
}

void VulkanFrame::DestroySwapchainDependentResources()
{
    for (FrameResources& frame : m_framesInFlight)
    {
        frame.guiColorImageView.reset();
        frame.guiColorImage.reset();
        frame.sceneColorImageView.reset();
        frame.sceneColorImage.reset();
    }
    m_presentCompleteSemaphores.clear();
}

void VulkanFrame::RecreateSwapchain()
{
    if (IsRecording())
    {
        RAD_LOG_GUI(warn, "VulkanFrame::RecreateSwapchain ignored while recording");
        return;
    }
    if (!m_window || !m_window->GetSurface())
    {
        return;
    }

    VulkanDevice* device = m_window->GetVulkanDevice();
    if (!device)
    {
        return;
    }

    device->WaitIdle();

    Ref<VulkanSwapchain> retiredSwapchain = std::move(m_swapchain);
    const vk::SwapchainKHR oldSwapchainHandle =
        retiredSwapchain ? retiredSwapchain->GetHandle() : VK_NULL_HANDLE;

    DestroySwapchainDependentResources();

    if (!CreateSwapchain(oldSwapchainHandle))
    {
        retiredSwapchain.reset();
        RAD_LOG_GUI(err, "VulkanFrame::RecreateSwapchain failed");
        return;
    }

    retiredSwapchain.reset();

    if (!CreateSwapchainDependentResources())
    {
        RAD_LOG_GUI(err, "VulkanFrame::RecreateSwapchain failed to rebuild resources");
    }
}

bool VulkanFrame::RebuildGui()
{
    if (!m_guiRenderer)
    {
        try
        {
            m_guiRenderer = RAD_NEW VulkanGuiRenderer(m_window, this);
            m_guiComposition = RAD_NEW VulkanGuiComposition(this);
        }
        catch (const std::exception& e)
        {
            RAD_LOG_GUI(err, "VulkanFrame failed to create GUI: {}", e.what());
            m_guiComposition.reset();
            m_guiRenderer.reset();
            return false;
        }
        return true;
    }

    m_guiRenderer->SetMinImageCount(m_swapchainMinImageCount);

    if (!m_guiComposition->OnSwapchainChanged(m_frameLag, m_swapchainColorFormat))
    {
        RAD_LOG_GUI(err, "VulkanFrame failed to rebuild the GUI composition pass");
        return false;
    }
    return true;
}

bool VulkanFrame::BeginFrame()
{
    if (!m_window)
    {
        return false;
    }

    VulkanDevice* device = m_window->GetVulkanDevice();
    VulkanSwapchain* swapchain = m_swapchain.get();
    if (!device || !swapchain || m_framesInFlight.empty())
    {
        return false;
    }

    // A zero-sized extent (e.g. a minimized window) cannot be presented; skip the frame so callers
    // don't have to special-case it.
    if (m_extent.width == 0 || m_extent.height == 0)
    {
        return false;
    }

    const uint32_t frameIndex = RecordingFrameIndex();
    FrameResources& frame = m_framesInFlight[frameIndex];

    CheckVulkanResult(frame.fence->Wait(), "VulkanFrame fence wait");

    const vk::Result acquireResult =
        swapchain->AcquireNextImage(UINT64_MAX, frame.imageAvailableSemaphore.get(), nullptr, 1);
    if (acquireResult == vk::Result::eErrorOutOfDateKHR)
    {
        RecreateSwapchain();
        return false;
    }
    if (acquireResult != vk::Result::eSuccess && acquireResult != vk::Result::eSuboptimalKHR)
    {
        RAD_LOG_GUI(err, "VulkanFrame AcquireNextImage failed: {}", vk::to_string(acquireResult));
        return false;
    }

    const uint32_t imageIndex = swapchain->GetCurrentImageIndex();
    if (imageIndex >= m_presentCompleteSemaphores.size())
    {
        return false;
    }

    // Reset the fence only once acquisition has succeeded and we are committed to submitting work
    // this frame. Resetting earlier would leave the fence unsignaled on the early-return paths
    // above (m_currentFrame is not advanced), deadlocking the next Wait() on this slot.
    frame.fence->Reset();

    m_frameState = VulkanFrameState::Recording;

    VulkanCommandBuffer* cmd = frame.commandBuffer.get();
    cmd->Reset(vk::CommandBufferResetFlags{});
    cmd->Begin();

    m_guiRenderer->BeginFrame();
    return true;
}

bool VulkanFrame::EndFrame(const VulkanFrameWaits* waits)
{
    if (!m_window)
    {
        return false;
    }

    VulkanDevice* device = m_window->GetVulkanDevice();
    VulkanSwapchain* swapchain = m_swapchain.get();
    if (!device || !swapchain || !IsRecording())
    {
        return false;
    }

    const uint32_t frameIndex = RecordingFrameIndex();
    if (frameIndex >= m_frameLag)
    {
        return false;
    }

    const uint32_t imageIndex = swapchain->GetCurrentImageIndex();
    if (imageIndex >= m_presentCompleteSemaphores.size())
    {
        m_frameState = VulkanFrameState::Idle;
        return false;
    }

    FrameResources& frame = m_framesInFlight[frameIndex];
    VulkanCommandBuffer* cmd = frame.commandBuffer.get();
    if (!cmd)
    {
        m_frameState = VulkanFrameState::Idle;
        return false;
    }

    if (waits && waits->waitRenderSubmitted)
    {
        waits->waitRenderSubmitted();
    }

    m_guiRenderer->Render();
    m_guiComposition->Record();

    cmd->End();

    const vk::detail::DispatchLoaderDynamic& dispatcher = device->GetDispatcher();
    vk::Queue queue = device->GetQueue(VulkanQueueFamily::Universal);

    VulkanSemaphore* presentSemaphore = m_presentCompleteSemaphores[imageIndex].get();
    vk::Semaphore waitSemaphores[2];
    vk::PipelineStageFlags waitStages[2];
    uint32_t waitSemaphoreCount = 0;

    waitSemaphores[waitSemaphoreCount] = frame.imageAvailableSemaphore->GetHandle();
    waitStages[waitSemaphoreCount] = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    ++waitSemaphoreCount;

    if (waits && waits->renderCompleteSemaphore)
    {
        waitSemaphores[waitSemaphoreCount] = waits->renderCompleteSemaphore->GetHandle();
        waitStages[waitSemaphoreCount] = waits->renderCompleteStage;
        ++waitSemaphoreCount;
    }

    vk::SubmitInfo submitInfo;
    submitInfo.waitSemaphoreCount = waitSemaphoreCount;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    const vk::CommandBuffer commandBuffers[] = {cmd->GetHandle()};
    submitInfo.pCommandBuffers = commandBuffers;
    submitInfo.signalSemaphoreCount = 1;
    const vk::Semaphore signalSemaphores[] = {presentSemaphore->GetHandle()};
    submitInfo.pSignalSemaphores = signalSemaphores;

    queue.submit(submitInfo, frame.fence->GetHandle(), dispatcher);

    vk::PresentInfoKHR presentInfo;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    presentInfo.swapchainCount = 1;
    const vk::SwapchainKHR swapchains[] = {swapchain->GetHandle()};
    presentInfo.pSwapchains = swapchains;
    presentInfo.pImageIndices = &imageIndex;

    const vk::Result presentResult = queue.presentKHR(presentInfo, dispatcher);
    if (presentResult == vk::Result::eErrorOutOfDateKHR)
    {
        m_frameState = VulkanFrameState::Idle;
        m_recreateSwapchainRequested = false;
        RecreateSwapchain();
        return false;
    }
    if (presentResult != vk::Result::eSuccess && presentResult != vk::Result::eSuboptimalKHR)
    {
        RAD_LOG_GUI(err, "VulkanFrame presentKHR failed: {}", vk::to_string(presentResult));
        m_frameState = VulkanFrameState::Idle;
        return false;
    }

    ++m_currentFrame;
    m_frameState = VulkanFrameState::Idle;
    if (m_recreateSwapchainRequested)
    {
        m_recreateSwapchainRequested = false;
        RecreateSwapchain();
    }
    return true;
}

bool VulkanFrame::SubmitFrame(std::function<void(VulkanFrameWaits*)> recordScene,
                              std::function<void()> buildGui)
{
    if (!BeginFrame())
    {
        return false;
    }

    VulkanFrameWaits waits;
    if (recordScene)
    {
        recordScene(&waits);
    }

    if (buildGui)
    {
        buildGui();
    }

    return EndFrame(recordScene ? &waits : nullptr);
}

} // namespace rad
