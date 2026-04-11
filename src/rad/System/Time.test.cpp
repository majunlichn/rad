#include <rad/System/Time.h>

#include <rad/IO/Logging.h>

#include <gtest/gtest.h>

TEST(System, Time)
{
    SPDLOG_INFO("Time(UTC): {}", rad::GetTimeStringUTC());
    SPDLOG_INFO("Time(ISO8601): {}", rad::GetTimeStringISO8601());
}
