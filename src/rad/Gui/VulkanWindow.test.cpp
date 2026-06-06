#include <rad/Common/MemoryDebug.h>

#include <rad/Gui/VulkanGui.test.h>
#include <rad/Gui/VulkanWindow.test.h>

#include <rad/Gui/GuiColor.h>
#include <rad/Gui/GuiCommon.h>
#include <rad/Gui/VulkanFrame.h>
#include <rad/Gui/VulkanGuiRenderer.h>

#include <gtest/gtest.h>

#include "imgui.h"

#include <stdexcept>

using namespace rad;

namespace
{

constexpr float kSceneClearSrgb[4] = {0.0f, 0.0f, 0.0f, 1.0f};

constexpr ColorPrecision kAllColorPrecisions[] = {
    ColorPrecision::Low,
    ColorPrecision::Medium,
    ColorPrecision::High,
};

void RecordScene(VulkanFrame* frame, const float clearSrgb[4])
{
    VulkanCommandBuffer* cmd = frame->GetCommandBuffer();
    VulkanImage* sceneColorImage = frame->GetSceneColorImage();
    VulkanImageView* sceneColorImageView = frame->GetSceneColorImageView();

    cmd->TransitionImageLayout(sceneColorImage, vk::ImageLayout::eColorAttachmentOptimal,
                               vk::PipelineStageFlagBits2::eColorAttachmentOutput,
                               vk::AccessFlagBits2::eColorAttachmentWrite);
    const VulkanImageView* colorViews[] = {sceneColorImageView};
    const vk::AttachmentLoadOp colorLoadOps[] = {vk::AttachmentLoadOp::eClear};
    const vk::AttachmentStoreOp colorStoreOps[] = {vk::AttachmentStoreOp::eStore};
    vk::ClearValue colorClearValues[1] = {};
    colorClearValues[0].color =
        LinearClearFromSrgb(clearSrgb[0], clearSrgb[1], clearSrgb[2], clearSrgb[3]);
    const vk::Rect2D renderArea = {{0, 0}, frame->GetExtent()};
    cmd->BeginRendering(colorViews, colorLoadOps, colorStoreOps,
                        colorClearValues, nullptr, vk::AttachmentLoadOp::eClear,
                        vk::AttachmentStoreOp::eStore, vk::AttachmentLoadOp::eClear,
                        vk::AttachmentStoreOp::eStore, vk::ClearDepthStencilValue{1.f, 0},
                        &renderArea);
    cmd->EndRendering();
}

void ShowColorPrecisionCombo(VulkanFrame* frame)
{
    int precisionIndex = 1;
    for (int i = 0; i < IM_ARRAYSIZE(kAllColorPrecisions); ++i)
    {
        if (kAllColorPrecisions[i] == frame->GetColorPrecision())
        {
            precisionIndex = i;
            break;
        }
    }
    static const char* const precisionLabels[] = {
        ColorPrecisionName(ColorPrecision::Low),
        ColorPrecisionName(ColorPrecision::Medium),
        ColorPrecisionName(ColorPrecision::High),
    };
    ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
    if (ImGui::Combo("Color Precision", &precisionIndex, precisionLabels,
                     IM_ARRAYSIZE(precisionLabels)))
    {
        frame->SetColorPrecision(kAllColorPrecisions[precisionIndex]);
    }
}

} // namespace

VulkanWindowTest::VulkanWindowTest() = default;

VulkanWindowTest::~VulkanWindowTest()
{
    Destroy();
}

bool VulkanWindowTest::Init(Ref<VulkanInstance> instance, Ref<VulkanDevice> device, int width,
                            int height, int maxFrames)
{
    m_maxFrames = maxFrames;
    m_frameCount = 0;

    VulkanWindow::CreateInfo info = {};
    info.instance = std::move(instance);
    info.device = std::move(device);
    info.title = "VulkanWindowTest";
    info.width = width;
    info.height = height;

    if (!Create(info))
    {
        RAD_LOG_GUI(err, "Failed to create VulkanWindowTest");
        return false;
    }

    try
    {
        VulkanFrame::Settings frameSettings = {};
        frameSettings.presentMode = vk::PresentModeKHR::eFifo;
        m_frame = RAD_NEW VulkanFrame(this, frameSettings);
    }
    catch (const std::exception&)
    {
        m_frame.reset();
        RAD_LOG_GUI(err, "VulkanWindowTest failed to create VulkanFrame");
        Destroy();
        return false;
    }

    return true;
}

void VulkanWindowTest::Destroy()
{
    RAD_LOG_GUI(info, "VulkanWindowTest::Destroy");
    m_frame.reset();
    VulkanWindow::Destroy();
}

bool VulkanWindowTest::OnEvent(const SDL_Event& event)
{
    if (VulkanGuiRenderer* guiRenderer = m_frame ? m_frame->GetGuiRenderer() : nullptr)
    {
        guiRenderer->ProcessEvent(event);
    }
    return VulkanWindow::OnEvent(event);
}

void VulkanWindowTest::ShowFrameSettingsWindow()
{
    if (!m_frame)
    {
        return;
    }

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    const float minWidth = viewport->Size.x * 0.4f;
    ImGui::SetNextWindowSizeConstraints(ImVec2(minWidth, 150.0f), viewport->WorkSize);
    if (ImGui::Begin("Frame Settings"))
    {
        ShowColorPrecisionCombo(m_frame.get());

        const bool hdrAvailable = m_frame->IsHdrAvailable();
        bool hdrEnabled = m_frame->GetEnableHdr();
        ImGui::BeginDisabled(!hdrAvailable);
        if (ImGui::Checkbox("HDR", &hdrEnabled))
        {
            m_frame->SetEnableHdr(hdrEnabled);
        }
        if (!hdrAvailable)
        {
            ImGui::SetItemTooltip("HDR color space not available on this display");
        }
        ImGui::EndDisabled();

        if (m_frame->IsHdr())
        {
            float hdrReferenceWhiteNits = m_frame->GetHdrReferenceWhiteNits();
            if (ImGui::SliderFloat("White point (nits)", &hdrReferenceWhiteNits, 80.0f, 1000.0f,
                                   "%.0f"))
            {
                m_frame->SetHdrReferenceWhiteNits(hdrReferenceWhiteNits);
            }
        }
    }
    ImGui::End();
}

void VulkanWindowTest::OnIdle()
{
    if (!m_frame)
    {
        VulkanWindow::OnIdle();
        return;
    }

    if (SDL_Window* window = GetHandle())
    {
        if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)
        {
            VulkanWindow::OnIdle();
            return;
        }
    }

    if (!m_frame->SubmitFrame(
            [this](VulkanFrameWaits*)
            { RecordScene(m_frame.get(), kSceneClearSrgb); },
            [this]()
            {
                ShowFrameSettingsWindow();
                if (m_showDemoWindow)
                {
                    ImGui::ShowDemoWindow(&m_showDemoWindow);
                }
            }))
    {
        VulkanWindow::OnIdle();
        return;
    }

    VulkanWindow::OnIdle();

    if (m_maxFrames >= 0)
    {
        ++m_frameCount;
        if (m_frameCount >= m_maxFrames)
        {
            Destroy();
        }
    }
}

void VulkanWindowTest::OnResized(int width, int height)
{
    Window::OnResized(width, height);
    RAD_LOG_GUI(info, "VulkanWindowTest::OnResized: {:4}, {:4}", width, height);
}

void VulkanWindowTest::OnPixelSizeChanged(int width, int height)
{
    Window::OnPixelSizeChanged(width, height);
    if (m_frame)
    {
        m_frame->RecreateSwapchain();
    }
    RAD_LOG_GUI(info, "VulkanWindowTest::OnPixelSizeChanged: {:4}, {:4}", width, height);
}

void VulkanWindowTest::OnCloseRequested()
{
    RAD_LOG_GUI(info, "VulkanWindowTest::OnCloseRequested");
    Destroy();
}

void VulkanWindowTest::OnKeyDown(const SDL_KeyboardEvent& keyDown)
{
    if (keyDown.key == SDLK_F1 && !keyDown.repeat)
    {
        m_showDemoWindow = !m_showDemoWindow;
    }
    else if (keyDown.key == SDLK_ESCAPE && !keyDown.repeat)
    {
        Destroy();
    }
    Window::OnKeyDown(keyDown);
}
