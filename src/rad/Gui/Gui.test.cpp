#include <rad/Common/MemoryDebug.h>

#include <rad/Gui/Gui.test.h>

TestEnvironment::TestEnvironment(int argc, char** argv) :
    m_argc(argc),
    m_argv(argv)
{
}

void TestEnvironment::SetUp()
{
    rad::GuiApplication* app = rad::GuiApplication::GetInstance();
    app->Init(m_argc, m_argv);
    m_window = RAD_NEW rad::Window();
    m_window->Create("GuiTest", 1920, 1080);
    app->Run();
}

void TestEnvironment::TearDown()
{
}

TestEnvironment* g_env = nullptr;

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    g_env = RAD_NEW TestEnvironment(argc, argv);
    testing::AddGlobalTestEnvironment(g_env);
    return RUN_ALL_TESTS();
}
