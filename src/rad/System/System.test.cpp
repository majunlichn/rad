#include <rad/System/System.test.h>

TestEnvironment::TestEnvironment(int argc, char** argv) :
    m_argc(argc),
    m_argv(argv)
{
}

void TestEnvironment::SetUp()
{
    m_app.Init(m_argc, m_argv);
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
