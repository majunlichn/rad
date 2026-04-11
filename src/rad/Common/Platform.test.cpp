#include <rad/Common/Platform.h>

#include <rad/IO/Logging.h>

#include <gtest/gtest.h>

TEST(Common, Platform)
{
#if defined(RAD_OS_WINDOWS) && defined(RAD_COMPILER_MSVC)
    SPDLOG_INFO("Compiled on Windows use Microsoft C++ (MSVC) compiler.");
#endif
}
