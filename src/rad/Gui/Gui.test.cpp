#include <rad/Common/MemoryDebug.h>

#include <rad/Gui/Gui.test.h>

#include <cstddef>
#include <cstdlib>
#include <cstring>

TestEnvironment::TestEnvironment(int argc, char** argv) :
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

void TestEnvironment::SetUp()
{
    ASSERT_TRUE(m_app.Init(m_argc, m_argv));
    ASSERT_EQ(rad::GuiApplication::GetInstance(), &m_app);
    m_window = RAD_NEW WindowTest();
    ASSERT_TRUE(m_window->Init(1920, 1080, m_maxFrames));
    m_app.Run();
}

void TestEnvironment::TearDown()
{
    m_window.reset();
    m_app.Destroy();
}

TestEnvironment* g_env = nullptr;

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    g_env = RAD_NEW TestEnvironment(argc, argv);
    testing::AddGlobalTestEnvironment(g_env);
    return RUN_ALL_TESTS();
}
