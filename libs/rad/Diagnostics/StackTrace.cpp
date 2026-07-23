#include <rad/Diagnostics/StackTrace.h>

#include <limits>

namespace rad
{

boost::stacktrace::stacktrace CaptureStackTrace(std::size_t maxDepth, std::size_t skipFrames)
{
    if ((maxDepth == 0) || (skipFrames == std::numeric_limits<std::size_t>::max()))
    {
        return boost::stacktrace::stacktrace(0, 0);
    }

    constexpr std::size_t skipThisFrame = 1;
    return boost::stacktrace::stacktrace(skipThisFrame + skipFrames, maxDepth);
}

std::string FormatStackTrace(const boost::stacktrace::stacktrace& stackTrace)
{
    return boost::stacktrace::to_string(stackTrace);
}

std::string GetStackTrace(std::size_t maxDepth)
{
    constexpr std::size_t skipThisFrame = 1;
    return FormatStackTrace(CaptureStackTrace(maxDepth, skipThisFrame));
}

} // namespace rad
