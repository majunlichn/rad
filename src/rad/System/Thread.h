#pragma once

#include <rad/Common/Platform.h>

#include <cstdint>
#include <string>
#include <string_view>

namespace rad
{

bool SetThreadName(cstring_view name);
std::string GetThreadName();

uint64_t GetCurrentThreadId();

} // namespace rad
