#pragma once

#include <cstddef>
#include <string>

namespace rad
{

// Captures and formats up to maxDepth frames from the current thread.
// Symbol names and source locations are included when debug information is available. Leading
// implementation frames may appear depending on compiler inlining. Returns an empty string when
// maxDepth is zero.
[[nodiscard]] std::string GetStackTrace(std::size_t maxDepth = 32);

} // namespace rad
