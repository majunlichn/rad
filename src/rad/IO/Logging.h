#pragma once

#include <rad/Common/String.h>

#include <spdlog/spdlog.h>

// https://fmt.dev/dev/api/
#include <fmt/std.h>

namespace rad
{

bool InitLogging(spdlog::sink_ptr fileSink = nullptr);
bool InitLogging(const std::string& filename, bool truncate);

spdlog::logger* GetDefaultLogger();
std::shared_ptr<spdlog::logger> CreateLogger(const std::string& name);

} // namespace rad
