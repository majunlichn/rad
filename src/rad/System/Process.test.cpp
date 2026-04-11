#include <rad/System/Process.h>

#include <rad/IO/Logging.h>

#include <gtest/gtest.h>

TEST(System, Process)
{
#if defined(RAD_OS_WINDOWS)
    auto outputFuture = rad::Process::ExecuteAndCaptureOutput("cmd.exe", {"/C", "echo Hello"});
    EXPECT_TRUE(outputFuture.get().starts_with("Hello"));
#else
    auto outputFuture = rad::Process::ExecuteAndCaptureOutput("echo", {"Hello"});
    EXPECT_TRUE(outputFuture.get().starts_with("Hello"));
#endif
}
