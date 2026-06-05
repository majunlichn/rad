#pragma once

#include <rad/Gui/VulkanFrame.h>
#include <rad/Gui/VulkanWindow.h>

class VulkanWindowTest : public rad::VulkanWindow
{
public:
    VulkanWindowTest();
    ~VulkanWindowTest() override;

    void Destroy() override;

    bool Init(rad::Ref<rad::VulkanInstance> instance, rad::Ref<rad::VulkanDevice> device, int width,
              int height, int maxFrames = -1);

    bool OnEvent(const SDL_Event& event) override;
    void OnIdle() override;

    void OnResized(int width, int height) override;
    void OnPixelSizeChanged(int width, int height) override;
    void OnCloseRequested() override;
    void OnKeyDown(const SDL_KeyboardEvent& keyDown) override;

private:
    void ShowFrameSettingsWindow();

    int m_maxFrames = -1;
    int m_frameCount = 0;

    rad::Ref<rad::VulkanFrame> m_frame;

    bool m_showDemoWindow = false;

}; // class VulkanWindowTest
