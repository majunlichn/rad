#include <rad/IO/IO.test.h>

TestEnvironment::TestEnvironment(int argc, char** argv)
{
}

void TestEnvironment::SetUp()
{
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
