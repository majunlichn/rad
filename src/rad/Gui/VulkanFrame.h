#pragma once

#include <rad/Gui/GuiCommon.h>
#include <rad/Vulkan/VulkanCore.h>

#include <functional>

namespace rad
{

class VulkanWindow;
class VulkanGuiRenderer;
class VulkanGuiComposition;
class VulkanSemaphore;

// Optional waits for an external scene queue.submit. Pass to EndFrame() when using multiple queues
// or rendering the scene on another thread.
struct VulkanFrameWaits
{
    // Blocks until an external scene submit has completed (e.g. another thread / queue).
    std::function<void()> waitRenderSubmitted;
    VulkanSemaphore* renderCompleteSemaphore = nullptr;
    vk::PipelineStageFlags renderCompleteStage = vk::PipelineStageFlagBits::eFragmentShader;
};

enum class VulkanFrameState
{
    Idle,
    Recording,
};

// Presentation core: swapchain, in-flight sync, scene color targets, present, and the GUI pipeline
// (VulkanGuiRenderer + VulkanGuiComposition). Construct after VulkanWindow has a valid surface.
// Call RecreateSwapchain() when the surface size or color settings change.
class VulkanFrame : public RefCounted<VulkanFrame>
{
public:
    // In-flight CPU frame slots (2 or 3). Double is always valid. Triple requires the surface to
    // allow three swapchain images; otherwise creation falls back to double.
    enum class BufferingMode
    {
        Double,
        Triple,
    };

    struct Settings
    {
        vk::PresentModeKHR presentMode = vk::PresentModeKHR::eFifo;
        // Preferred swapchain image count; clamped to surface minImageCount / maxImageCount.
        uint32_t swapchainImageCount = 3;
        BufferingMode buffering = BufferingMode::Triple;
        ColorPrecision colorPrecision = ColorPrecision::Medium;
        bool enableHdr = false;
        // Luminance (cd/m^2) that SDR-referred white (linear 1.0) maps to when presenting HDR.
        // Ignored for SDR modes. 203 nits matches ITU-R BT.2408 graphics white.
        float hdrReferenceWhiteNits = 203.0f;
    };

    explicit VulkanFrame(VulkanWindow* window);
    explicit VulkanFrame(VulkanWindow* window, const Settings& settings);
    ~VulkanFrame();

    VulkanFrame(const VulkanFrame&) = delete;
    VulkanFrame& operator=(const VulkanFrame&) = delete;

    VulkanWindow* GetWindow() const { return m_window; }
    VulkanSwapchain* GetSwapchain() const { return m_swapchain.get(); }
    VulkanGuiRenderer* GetGuiRenderer() const { return m_guiRenderer.get(); }
    VulkanGuiComposition* GetGuiComposition() const { return m_guiComposition.get(); }
    uint32_t GetSwapchainMinImageCount() const { return m_swapchainMinImageCount; }
    uint32_t GetFrameLag() const { return m_frameLag; }
    BufferingMode GetBufferingMode() const { return m_buffering; }
    vk::PresentModeKHR GetPresentMode() const { return m_presentMode; }
    float GetHdrReferenceWhiteNits() const { return m_hdrReferenceWhiteNits; }

    void SetBufferingMode(BufferingMode mode);
    // Double is always supported. Triple is supported only when the surface allows three swapchain
    // images (see GetSwapchain()->GetImageCount() after creation).
    bool IsBufferingModeSupported(BufferingMode mode) const;

    // Updates the SDR-white luminance used by HDR encodings. Takes effect on the next composite;
    // no swapchain recreation. Values <= 0 are ignored.
    void SetHdrReferenceWhiteNits(float nits);

    // The following recreate the swapchain when not recording. If a frame is open, the change is
    // deferred until after the current frame is presented.
    bool SetPresentMode(vk::PresentModeKHR presentMode);
    // Clamped to at least GetFrameLag(); deferred while recording like other swapchain settings.
    bool SetSwapchainImageCount(uint32_t imageCount);
    bool SetEnableHdr(bool enableHdr);
    bool SetColorPrecision(ColorPrecision precision);
    // True when the surface advertises HDR10 or scRGB (not SDR fallback).
    bool IsHdrAvailable() const;
    bool GetEnableHdr() const { return m_enableHdr; }
    ColorPrecision GetColorPrecision() const { return m_colorPrecision; }

    vk::Extent2D GetExtent() const { return m_extent; }
    vk::Format GetSwapchainColorFormat() const { return m_swapchainColorFormat; }
    vk::ColorSpaceKHR GetSwapchainColorSpace() const { return m_swapchainColorSpace; }
    // Linear scene color format (see ColorPrecision; HDR always RGBA16F).
    vk::Format GetSceneColorFormat() const { return m_sceneColorFormat; }

    // Fixed R8G8B8A8_UNORM GUI color targets (see GetGuiColorFormat()).
    static vk::Format GetGuiColorFormat() { return vk::Format::eR8G8B8A8Unorm; }

    static bool IsHdrColorSpace(vk::ColorSpaceKHR colorSpace);

    // True when the active swapchain presents in an HDR color space (scene targets are RGBA16F).
    bool IsHdr() const { return IsHdrColorSpace(m_swapchainColorSpace); }

    VulkanFrameState GetState() const { return m_frameState; }
    // True between BeginFrame() and EndFrame() (VulkanFrameState::Recording).
    bool IsRecording() const { return m_frameState == VulkanFrameState::Recording; }
    // In-flight slot being recorded; only valid while IsRecording().
    uint32_t GetFrameIndex() const;

    VulkanCommandBuffer* GetCommandBuffer() const;
    VulkanImage* GetSceneColorImage() const;
    VulkanImageView* GetSceneColorImageView() const;
    VulkanImage* GetSceneColorImage(uint32_t frameIndex) const;
    VulkanImageView* GetSceneColorImageView(uint32_t frameIndex) const;

    VulkanImage* GetGuiColorImage() const;
    VulkanImageView* GetGuiColorImageView() const;
    VulkanImage* GetGuiColorImage(uint32_t frameIndex) const;
    VulkanImageView* GetGuiColorImageView(uint32_t frameIndex) const;

    // The acquired swapchain image / view for the frame being recorded. Null when not recording.
    VulkanImage* GetSwapchainImage() const;
    VulkanImageView* GetSwapchainImageView() const;

    // Acquire swapchain image, reset the command buffer, and start the ImGui frame.
    bool BeginFrame();
    // Render GUI, composite to the swapchain image, submit, and present.
    bool EndFrame(const VulkanFrameWaits* waits = nullptr);

    // Acquire, record scene (optional), build ImGui (optional), then EndFrame() renders GUI and presents.
    // recordScene may fill waits when the scene pass uses its own queue submit.
    bool SubmitFrame(std::function<void(VulkanFrameWaits*)> recordScene = {},
                     std::function<void()> buildGui = {});

    // Recreates the swapchain and rebuilds scene targets plus the GUI pipeline. No-op while
    // recording; use RequestRecreateSwapchain() to defer until after present.
    void RecreateSwapchain();

private:
    struct FrameResources
    {
        Ref<VulkanCommandBuffer> commandBuffer;
        Ref<VulkanSemaphore> imageAvailableSemaphore;
        Ref<VulkanFence> fence;
        Ref<VulkanImage> sceneColorImage;
        Ref<VulkanImageView> sceneColorImageView;
        Ref<VulkanImage> guiColorImage;
        Ref<VulkanImageView> guiColorImageView;
    };

    bool CreateSwapchain(vk::SwapchainKHR oldSwapchain = VK_NULL_HANDLE);
    void DestroySwapchain();

    bool InitFrameSync();
    void DestroyFrameSync();

    bool CreateRenderTargets();
    bool CreatePresentCompleteSemaphores();
    void ClearFrameRenderTargets(FrameResources& frame);

    // Creates the GUI pipeline on first call; resizes it after swapchain recreation.
    bool RebuildGui();
    void RequestRecreateSwapchain();

    static uint32_t GetFrameLag(BufferingMode mode);
    static bool HasExternalWaits(const VulkanFrameWaits& waits);

    uint32_t RecordingFrameIndex() const { return m_currentFrame % m_frameLag; }

    bool CreateSwapchainDependentResources();
    void DestroySwapchainDependentResources();
    void Shutdown();

    VulkanWindow* m_window = nullptr;
    Ref<VulkanSwapchain> m_swapchain;
    Ref<VulkanGuiRenderer> m_guiRenderer;
    Ref<VulkanGuiComposition> m_guiComposition;

    vk::PresentModeKHR m_presentMode = vk::PresentModeKHR::eFifo;
    uint32_t m_desiredSwapchainImageCount = 3;
    uint32_t m_swapchainMinImageCount = 2;

    BufferingMode m_buffering = BufferingMode::Triple;
    ColorPrecision m_colorPrecision = ColorPrecision::Medium;
    bool m_enableHdr = false;
    float m_hdrReferenceWhiteNits = 203.0f;
    uint32_t m_frameLag = 3;
    uint32_t m_currentFrame = 0;

    VulkanFrameState m_frameState = VulkanFrameState::Idle;
    bool m_recreateSwapchainRequested = false;

    Ref<VulkanCommandPool> m_commandPool;
    std::vector<FrameResources> m_framesInFlight;
    std::vector<Ref<VulkanSemaphore>> m_presentCompleteSemaphores;

    vk::Format m_swapchainColorFormat = vk::Format::eUndefined;
    vk::ColorSpaceKHR m_swapchainColorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;
    vk::Format m_sceneColorFormat = vk::Format::eUndefined;
    vk::Extent2D m_extent = {};

}; // class VulkanFrame

} // namespace rad
