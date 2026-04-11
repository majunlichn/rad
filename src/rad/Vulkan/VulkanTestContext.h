#pragma once

#include <rad/Vulkan/VulkanInstance.h>

#include <rad/System/Application.h>
#include <rad/IO/Logging.h>

#include <gtest/gtest.h>

class VulkanTestContext : public ::testing::Environment
{
public:
    int m_argc = 0;
    char** m_argv = nullptr;
    rad::Application m_app;
    rad::Ref<rad::VulkanInstance> m_instance;

    VulkanTestContext(int argc, char** argv) :
        m_argc(argc),
        m_argv(argv)
    {
        m_app.Init(m_argc, m_argv);
    }

    void SetUp() override
    {
        m_instance = rad::VulkanInstance::Create("VulkanTests", VK_MAKE_VERSION(1, 0, 0), "Rad",
                                                 VK_MAKE_VERSION(1, 0, 0));
    }

    void TearDown() override {}

}; // TestContext

extern VulkanTestContext* g_context;
