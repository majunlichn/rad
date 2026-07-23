#pragma once

#include <boost/stacktrace/stacktrace.hpp>

#include <cstddef>
#include <string>

namespace rad
{

// Captures up to maxDepth frames from the current thread. skipFrames can be used by wrappers to
// omit their own implementation frames.
[[nodiscard]] boost::stacktrace::stacktrace CaptureStackTrace(
    std::size_t maxDepth = 32, std::size_t skipFrames = 0);

// Formats a previously captured trace. Symbol names and source locations are included when debug
// information is available.
[[nodiscard]] std::string FormatStackTrace(const boost::stacktrace::stacktrace& stackTrace);

// Captures and formats up to maxDepth frames from the current thread. Leading implementation
// frames may appear depending on compiler inlining. Returns an empty string when maxDepth is zero.
[[nodiscard]] std::string GetStackTrace(std::size_t maxDepth = 32);

} // namespace rad
