#include <rad/Common/MemoryDebug.h>

#include <rad/Multimedia/Multimedia.test.h>

TestEnvironment::TestEnvironment(int argc, char** argv) :
    m_argc(argc),
    m_argv(argv)
{
}

void TestEnvironment::SetUp()
{
    rad::GuiApplication* app = rad::GuiApplication::GetInstance();
    ASSERT_TRUE(app->Init(m_argc, m_argv));
}

void TestEnvironment::TearDown()
{
    rad::GuiApplication::GetInstance()->Destroy();
}

TestEnvironment* g_env = nullptr;

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    g_env = RAD_NEW TestEnvironment(argc, argv);
    testing::AddGlobalTestEnvironment(g_env);
    return RUN_ALL_TESTS();
}
