#pragma once

#include <rad/System/Application.h>
#include <rad/IO/Logging.h>

#include <rad/Vulkan/VulkanInstance.h>
#include <rad/Vulkan/VulkanDevice.h>

#include <gtest/gtest.h>

class TestEnvironment : public ::testing::Environment
{
public:
    TestEnvironment(int argc, char** argv);

    void SetUp() override;
    void TearDown() override;

    rad::Application& GetApp() { return m_app; }

private:
    int m_argc = 0;
    char** m_argv = nullptr;
    rad::Application m_app;
    rad::Ref<rad::VulkanInstance> m_instance;
    rad::Ref<rad::VulkanDevice> m_device;

}; // TestEnvironment

extern TestEnvironment* g_env;
