#include <rad/Vulkan/VulkanTestContext.h>

VulkanTestContext* g_context = nullptr;

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    g_context = new VulkanTestContext(argc, argv);
    testing::AddGlobalTestEnvironment(g_context);
    return RUN_ALL_TESTS();
}
