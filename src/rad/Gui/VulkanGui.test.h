#pragma once

#include <rad/Gui/GuiApplication.h>
#include <rad/Vulkan/VulkanCore.h>

#include "VulkanWindow.test.h"

#include <gtest/gtest.h>

class VulkanGuiTestEnvironment : public ::testing::Environment
{
public:
    /// If argv contains a well-formed `--max-frames=N`, assigns N to m_maxFrames; otherwise leaves
    /// m_maxFrames at its initial value (-1).
    VulkanGuiTestEnvironment(int argc, char** argv);

    void SetUp() override;
    void TearDown() override;

    rad::GuiApplication* GetApp() { return rad::GuiApplication::GetInstance(); }
    rad::VulkanInstance* GetInstance() { return m_instance.get(); }
    rad::VulkanDevice* GetDevice() { return m_device.get(); }

private:
    bool CreateVulkanDevice();

    int m_argc = 0;
    char** m_argv = nullptr;
    int m_maxFrames = -1;
    rad::GuiApplication m_app;
    rad::Ref<rad::VulkanInstance> m_instance;
    rad::Ref<rad::VulkanDevice> m_device;
    rad::Ref<VulkanWindowTest> m_window;
}; // class VulkanGuiTestEnvironment

extern VulkanGuiTestEnvironment* g_vulkanGuiEnv;
