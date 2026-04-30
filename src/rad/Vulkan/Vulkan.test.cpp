#include <rad/Vulkan/Vulkan.test.h>

TestEnvironment::TestEnvironment(int argc, char** argv) :
    m_argc(argc),
    m_argv(argv)
{
}

void TestEnvironment::SetUp()
{
    m_app.Init(m_argc, m_argv);
    m_instance = rad::VulkanInstance::Create("Vulkan Tests", VK_MAKE_VERSION(1, 0, 0), "rad",
                                             VK_MAKE_VERSION(1, 0, 0));
}

void TestEnvironment::TearDown()
{
}

TestEnvironment* g_env = nullptr;

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    g_env = new TestEnvironment(argc, argv);
    testing::AddGlobalTestEnvironment(g_env);
    return RUN_ALL_TESTS();
}
