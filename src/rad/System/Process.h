#pragma once

#include <rad/Common/Platform.h>
#include <rad/Common/String.h>
#include <rad/System/OS.h>
#include <rad/System/Thread.h>

#include <future>

namespace rad
{

class Process
{
public:
    static std::future<std::string> ExecuteAndCaptureOutput(
        const std::string& executable, const std::vector<std::string>& args,
        std::chrono::milliseconds timeout = std::chrono::milliseconds::max());

}; // namespace rad

} // namespace rad
