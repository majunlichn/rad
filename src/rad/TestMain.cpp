#include <rad/TestContext.h>

TestContext* g_context = nullptr;

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    g_context = new TestContext(argc, argv);
    testing::AddGlobalTestEnvironment(g_context);
    return RUN_ALL_TESTS();
}
