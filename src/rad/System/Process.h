#pragma once

#include <rad/Common/Platform.h>
#include <rad/Common/String.h>

#include <chrono>

namespace rad
{

class Process
{
public:
    static std::string ExecuteAndCaptureOutput(const std::string& executable,
                                               const std::vector<std::string>& args,
                                               std::optional<std::chrono::milliseconds> timeout);

}; // class Process

} // namespace rad
