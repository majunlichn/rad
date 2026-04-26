#pragma once

#include <rad/Common/Platform.h>
#include <rad/Common/TypeTraits.h>

#include <cassert>
#include <cstdint>
#include <ctime>

#include <chrono>

namespace rad
{

using Nanoseconds = std::chrono::nanoseconds;
using Microseconds = std::chrono::microseconds;
using Milliseconds = std::chrono::milliseconds;
using Seconds = std::chrono::seconds;
using Minutes = std::chrono::minutes;
using Hours = std::chrono::hours;
using Days = std::chrono::days;
using Weeks = std::chrono::weeks;
using Months = std::chrono::months;
using Years = std::chrono::years;

// Clock type for measuring performance, guaranteed to be steady.
using PerfClock = std::conditional_t<std::chrono::high_resolution_clock::is_steady,
                                     std::chrono::high_resolution_clock, std::chrono::steady_clock>;

struct tm* LocalTime(const time_t* timer, struct tm* buffer);

// Returns string in format "YYYY-MM-DDThh:mm:ss.sssZ" or empty if failed.
std::string GetTimeStringUTC();
// Returns string in format "YYYY-MM-DDThh:mm:ss.sss+0000" or empty if failed.
std::string GetTimeStringISO8601();

} // namespace rad
