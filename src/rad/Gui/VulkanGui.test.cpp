#include <rad/Common/MemoryDebug.h>

#include <rad/Gui/VulkanGui.test.h>

#include <SDL3/SDL_vulkan.h>

#include <cstddef>
#include <cstdlib>
#include <cstring>

VulkanGuiTestEnvironment::VulkanGuiTestEnvironment(int argc, char** argv) :
    m_argc(argc),
    m_argv(argv)
{
    constexpr std::size_t kMaxFramesArgPrefixLen = sizeof("--max-frames=") - 1;
    for (int i = 1; i < argc; ++i)
    {
        const char* arg = argv[i];
        if (std::strncmp(arg, "--max-frames=", kMaxFramesArgPrefixLen) == 0)
        {
            const char* value = arg + kMaxFramesArgPrefixLen;
            char* end = nullptr;
            const long parsed = std::strtol(value, &end, 10);
            if (end != value)
            {
                m_maxFrames = static_cast<int>(parsed);
            }
        }
    }
}

bool VulkanGuiTestEnvironment::CreateVulkanDevice()
{
    Uint32 sdlExtensionCount = 0;
    const char* const* sdlExtensions = SDL_Vulkan_GetInstanceExtensions(&sdlExtensionCount);
    if (!sdlExtensions)
    {
        RAD_LOG_GUI(err, "SDL_Vulkan_GetInstanceExtensions failed: {}", SDL_GetError());
        return false;
    }

    rad::VulkanInstanceConfig instanceConfig;
    instanceConfig.enableValidationLayer = false;
    for (Uint32 i = 0; i < sdlExtensionCount; ++i)
    {
        instanceConfig.extensions.insert(sdlExtensions[i]);
    }

    m_instance = rad::VulkanInstance::Create("VulkanGui", VK_MAKE_VERSION(1, 0, 0), "rad",
                                           VK_MAKE_VERSION(1, 0, 0), instanceConfig);
    if (!m_instance)
    {
        RAD_LOG_GUI(err, "VulkanInstance::Create failed");
        return false;
    }

    const std::vector<vk::PhysicalDevice>& physicalDevices = m_instance->GetPhysicalDevices();
    if (physicalDevices.empty())
    {
        RAD_LOG_GUI(err, "No Vulkan physical devices found");
        return false;
    }

    rad::VulkanDeviceConfig deviceConfig;
    deviceConfig.extensions.insert(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    if (m_instance->GetApiVersion().IsGreaterEqualThan(1, 3, 0))
    {
        deviceConfig.enableVulkan13Features = true;
    }
    else
    {
        deviceConfig.extensions.insert(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME);
        deviceConfig.enableVulkan12Features = true;
    }

    m_device = RAD_NEW rad::VulkanDevice(m_instance, physicalDevices.front(), deviceConfig);
    return m_device != nullptr;
}

void VulkanGuiTestEnvironment::SetUp()
{
    ASSERT_TRUE(m_app.Init(m_argc, m_argv));
    ASSERT_EQ(rad::GuiApplication::GetInstance(), &m_app);
    ASSERT_TRUE(CreateVulkanDevice());

    m_window = RAD_NEW VulkanWindowTest();
    ASSERT_TRUE(m_window->Init(m_instance, m_device, 1920, 1080, m_maxFrames));
    m_app.Run();
}

void VulkanGuiTestEnvironment::TearDown()
{
    m_window.reset();
    m_device.reset();
    m_instance.reset();
    m_app.Destroy();
}

VulkanGuiTestEnvironment* g_vulkanGuiEnv = nullptr;

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    g_vulkanGuiEnv = RAD_NEW VulkanGuiTestEnvironment(argc, argv);
    testing::AddGlobalTestEnvironment(g_vulkanGuiEnv);
    return RUN_ALL_TESTS();
}

TEST(VulkanGui, CheckExtensions)
{
    ASSERT_NE(g_vulkanGuiEnv->GetInstance(), nullptr);
    ASSERT_NE(g_vulkanGuiEnv->GetDevice(), nullptr);
    EXPECT_TRUE(g_vulkanGuiEnv->GetInstance()->IsExtensionEnabled(VK_KHR_SURFACE_EXTENSION_NAME));
    EXPECT_TRUE(g_vulkanGuiEnv->GetDevice()->IsExtensionEnabled(VK_KHR_SWAPCHAIN_EXTENSION_NAME));
}
