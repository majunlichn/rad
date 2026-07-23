#include <rad/Core/Platform.h>
#include <rad/System/StackTrace.h>

#include <gtest/gtest.h>

#include <string>

namespace
{

RAD_NOINLINE std::string CaptureStackTrace(std::size_t maxDepth = 32)
{
    return rad::GetStackTrace(maxDepth);
}

} // namespace

TEST(System, StackTrace)
{
    const std::string trace = CaptureStackTrace();

    EXPECT_FALSE(trace.empty());
    EXPECT_FALSE(CaptureStackTrace(1).empty());

#if defined(_WIN32)
    EXPECT_NE(trace.find("CaptureStackTrace"), std::string::npos) << trace;
#endif

    EXPECT_TRUE(rad::GetStackTrace(0).empty());
}
