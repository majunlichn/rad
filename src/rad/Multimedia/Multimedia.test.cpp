#include <rad/Common/MemoryDebug.h>

#include <rad/Multimedia/Multimedia.test.h>

#include <rad/IO/Logging.h>

using namespace rad;

TestEnvironment::TestEnvironment(int argc, char** argv) :
    m_argc(argc),
    m_argv(argv)
{
}

void TestEnvironment::SetUp()
{
    std::string logFileName = pystring::os::path::basename(m_argv[0]) + ".log";
    InitLogging(logFileName, true);
}

void TestEnvironment::TearDown()
{
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    testing::AddGlobalTestEnvironment(RAD_NEW TestEnvironment(argc, argv));
    return RUN_ALL_TESTS();
}
