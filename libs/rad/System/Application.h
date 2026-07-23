#pragma once

#include <rad/IO/Logging.h>
#include <rad/System/OS.h>
#include <rad/System/SignalHandling.h>

#include <memory>
#include <string>
#include <vector>

namespace rad
{

// Configures process-wide application behavior. Call Init once during single-threaded startup.
class Application final
{
public:
    static void Init(int argc, char** argv);

    // Arguments are UTF-8 encoded and remain valid for the process lifetime.
    [[nodiscard]] static const std::vector<std::string>& Arguments() noexcept;
    [[nodiscard]] static spdlog::logger* GetLogger() noexcept;

    static void InstallDefaultSignalHandlers();
    static void InstallDefaultTerminateHandler() noexcept;

    [[noreturn]] static void Exit(int code = 0);
    [[noreturn]] static void QuickExit(int code = 0) noexcept;
    [[noreturn]] static void Terminate() noexcept;
    [[noreturn]] static void Abort() noexcept;

private:
    Application() = default;

    [[nodiscard]] static Application& Instance();
    static void DefaultSignalHandler(int signal) noexcept;
    static void DefaultTerminateHandler() noexcept;
    static void InitializeCommandLine(int argc, char** argv);
    static void InitializeLogging();
    static void ConfigureUtf8Locale();
    static void ConfigureUtf8Console();
    static void EnableDebugFeatures() noexcept;

    bool m_terminateHandlerInstalled = false;
    std::vector<std::string> m_arguments;
    std::shared_ptr<spdlog::logger> m_logger;
    std::unique_ptr<SignalHandling> m_signalHandling;
}; // class Application

} // namespace rad
