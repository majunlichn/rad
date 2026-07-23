#include <rad/System/StackTrace.h>

#include <boost/stacktrace/stacktrace.hpp>

namespace rad
{

std::string GetStackTrace(std::size_t maxDepth)
{
    if (maxDepth == 0)
    {
        return {};
    }

    constexpr std::size_t skipThisFrame = 1;
    return boost::stacktrace::to_string(
        boost::stacktrace::stacktrace(skipThisFrame, maxDepth));
}

} // namespace rad
