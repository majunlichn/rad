#include <rad/ML/ML.test.h>

#include <rad/IO/Logging.h>
#include <rad/ML/MLGlobal.h>

using namespace rad;

MLTestEnvironment* g_env = nullptr;

void MLTestEnvironment::SetUp()
{
    InitLogging();
    ASSERT_EQ(GetMLDevice("cpu"), nullptr);
    MLInit();
}

void MLTestEnvironment::TearDown()
{
    MLShutdown();
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    g_env = new MLTestEnvironment();
    testing::AddGlobalTestEnvironment(g_env);
    return RUN_ALL_TESTS();
}
