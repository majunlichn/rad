#pragma once

#include <rad/Common/Platform.h>
#include <rad/Common/String.h>

namespace rad
{

bool SetThreadName(cstring_view name);
std::string GetThreadName();

uint64_t GetCurrentThreadId();

} // namespace rad
