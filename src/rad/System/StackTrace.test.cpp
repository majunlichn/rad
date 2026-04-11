#include <rad/System/StackTrace.h>

#include <rad/IO/Logging.h>

#include <gtest/gtest.h>

TEST(System, StackTrace)
{
#if defined(_DEBUG)
    std::string trace = rad::PrintStackTrace(3);
    SPDLOG_INFO("Stack trace:\n{}", trace);
#endif
}
