#include <rad/System/Time.h>

#include <cassert>

#include <chrono>
#include <format>

namespace rad
{

tm* LocalTime(const time_t* timer, tm* buffer)
{
    assert((timer != nullptr) && (buffer != nullptr));
#if defined(RAD_OS_WINDOWS)
    errno_t err = localtime_s(buffer, timer);
    if (err == 0)
    {
        return buffer;
    }
    else
    {
        return nullptr;
    }
#else
    return localtime_r(timer, buffer);
#endif
}

std::string GetTimeStringUTC()
{
    try
    {
        auto now = std::chrono::system_clock::now();
        auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
        return std::format("{:%FT%TZ}", now_ms);
    }
    catch (const std::exception&)
    {
        return std::string();
    }
}

std::string GetTimeStringISO8601()
{
    try
    {
        auto now = std::chrono::system_clock::now();
        auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
        return std::format("{:%FT%T%z}",
                           std::chrono::zoned_time{std::chrono::current_zone(), now_ms});
    }
    catch (const std::exception&)
    {
        return std::string();
    }
}

} // namespace rad
