#include <rad/Core/Platform.h>
#include <rad/Diagnostics/StackTrace.h>

#include <gtest/gtest.h>

#include <string>

namespace
{

RAD_NOINLINE std::string CaptureFormattedStackTrace(std::size_t maxDepth = 32)
{
    return rad::GetStackTrace(maxDepth);
}

} // namespace

TEST(Diagnostics, StackTrace)
{
    const boost::stacktrace::stacktrace captured = rad::CaptureStackTrace();
    const std::string trace = CaptureFormattedStackTrace();

    EXPECT_FALSE(captured.empty());
    EXPECT_FALSE(rad::FormatStackTrace(captured).empty());
    EXPECT_FALSE(trace.empty());
    EXPECT_FALSE(CaptureFormattedStackTrace(1).empty());

#if defined(_WIN32) && defined(_DEBUG)
    EXPECT_NE(trace.find("CaptureFormattedStackTrace"), std::string::npos) << trace;
#endif

    EXPECT_TRUE(rad::CaptureStackTrace(0).empty());
    EXPECT_TRUE(rad::GetStackTrace(0).empty());
}
