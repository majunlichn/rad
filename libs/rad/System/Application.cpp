#include <rad/System/Application.h>

#include <rad/Core/MemoryDebug.h>
#include <rad/Core/Platform.h>
#include <rad/Diagnostics/Exception.h>
#include <rad/Diagnostics/StackTrace.h>
#include <rad/IO/Logging.h>

#include <boost/nowide/args.hpp>
#include <boost/nowide/filesystem.hpp>

#include <algorithm>
#include <cctype>
#include <clocale>
#include <csignal>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <locale>
#include <memory>
#include <stdexcept>
#include <string_view>
#include <system_error>
#include <utility>
#include <vector>

#if defined(RAD_OS_WINDOWS)
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace rad
{
namespace
{

bool IsUtf8Locale(std::string_view name)
{
    std::string normalized(name);
    std::transform(normalized.begin(), normalized.end(), normalized.begin(),
                   [](unsigned char character) { return static_cast<char>(std::tolower(character)); });
    return normalized.find("utf-8") != std::string::npos ||
           normalized.find("utf8") != std::string::npos;
}

void WriteSignalMessage(int signal) noexcept
{
    std::string_view message = "Application terminated by an unknown signal.\n";
    switch (signal)
    {
    case SIGINT:
        message = "Application terminated by SIGINT (interrupt request).\n";
        break;
    case SIGTERM:
        message = "Application terminated by SIGTERM (termination request).\n";
        break;
    default:
        break;
    }
#if defined(RAD_OS_WINDOWS)
    const HANDLE errorHandle = GetStdHandle(STD_ERROR_HANDLE);
    if (errorHandle != nullptr && errorHandle != INVALID_HANDLE_VALUE)
    {
        DWORD written = 0;
        WriteFile(errorHandle, message.data(), static_cast<DWORD>(message.size()), &written,
                  nullptr);
    }
#else
    const auto ignored = ::write(STDERR_FILENO, message.data(), message.size());
    static_cast<void>(ignored);
#endif
}

} // namespace

Application& Application::Instance()
{
    static Application application;
    return application;
}

void Application::DefaultSignalHandler(int signal) noexcept
{
    WriteSignalMessage(signal);
    std::_Exit(128 + signal);
}

void Application::DefaultTerminateHandler() noexcept
{
    std::string reason = "Unhandled exception";
    bool hasCapturedStackTrace = false;
    try
    {
        if (const std::exception_ptr exception = std::current_exception())
        {
            try
            {
                std::rethrow_exception(exception);
            }
            catch (const Exception& error)
            {
                reason += ":\n";
                reason += error.DiagnosticInformation();
                hasCapturedStackTrace = !error.StackTrace().empty();
            }
            catch (const std::exception& error)
            {
                reason += ": ";
                reason += error.what();
            }
            catch (...)
            {
                reason += " of unknown type";
            }
        }
        else
        {
            reason = "std::terminate called without an active exception";
        }

        if (!hasCapturedStackTrace)
        {
            reason += '\n';
            reason += GetStackTrace();
        }

        auto& application = Instance();
        if (application.m_logger)
        {
            application.m_logger->critical("{}", reason);
            application.m_logger->flush();
        }
        else
        {
            std::cerr << reason << std::endl;
        }
    }
    catch (...)
    {
        constexpr std::string_view fallback = "Application terminated unexpectedly.\n";
#if defined(RAD_OS_WINDOWS)
        const HANDLE errorHandle = GetStdHandle(STD_ERROR_HANDLE);
        if (errorHandle != nullptr && errorHandle != INVALID_HANDLE_VALUE)
        {
            DWORD written = 0;
            WriteFile(errorHandle, fallback.data(), static_cast<DWORD>(fallback.size()), &written,
                      nullptr);
        }
#else
        const auto ignored = ::write(STDERR_FILENO, fallback.data(), fallback.size());
        static_cast<void>(ignored);
#endif
    }

    std::abort();
}

void Application::Init(int argc, char** argv)
{
    InitializeCommandLine(argc, argv);

    ConfigureUtf8Locale();
    ConfigureUtf8Console();
    EnableDebugFeatures();

    InitializeLogging();
    InstallDefaultSignalHandlers();
    InstallDefaultTerminateHandler();

    if (auto* logger = GetLogger())
    {
        logger->info("Executable: {}", PathToUtf8(os::executable_path()));
#if defined(_DEBUG)
        const auto& args = Arguments();
        for (std::size_t index = 1; index < args.size(); ++index)
        {
            logger->debug("Argument[{}]: {}", index, args[index]);
        }
#endif
        logger->info("Working directory: {}", PathToUtf8(os::getcwd()));
        logger->info("Temporary directory: {}", PathToUtf8(os::temp_directory_path()));
    }
}

const std::vector<std::string>& Application::Arguments() noexcept
{
    return Instance().m_arguments;
}

spdlog::logger* Application::GetLogger() noexcept
{
    return Instance().m_logger.get();
}

void Application::InitializeLogging()
{
    auto& application = Instance();
    if (application.m_logger)
    {
        return;
    }

    auto& logManager = LogManager::Instance();
    const os::FilePath executablePath = os::executable_path();
    const bool logInitialized = logManager.IsInitialized();
    if (!logInitialized)
    {
        os::FilePath logName = executablePath.stem();
        logName += os::FilePath{".log"};
        logManager.Init(PathToUtf8(os::getcwd() / logName), true);
    }

    try
    {
        application.m_logger = logManager.CreateLogger(PathToUtf8(executablePath.stem()));
    }
    catch (...)
    {
        if (!logInitialized)
        {
            logManager.Shutdown();
        }
        throw;
    }
}

void Application::InstallDefaultSignalHandlers()
{
    auto& application = Instance();
    if (application.m_signalHandling)
    {
        return;
    }

    auto signalHandling = std::make_unique<SignalHandling>();
    if (!signalHandling->IsLoaded())
    {
        if (application.m_logger)
        {
            application.m_logger->warn("Fatal signal diagnostics are unavailable on this platform");
        }
    }
    if (std::signal(SIGINT, DefaultSignalHandler) == SIG_ERR ||
        std::signal(SIGTERM, DefaultSignalHandler) == SIG_ERR)
    {
        throw std::runtime_error{"Failed to install default signal handlers"};
    }
    application.m_signalHandling = std::move(signalHandling);
    if (application.m_logger)
    {
        application.m_logger->debug("Default signal handlers installed");
    }
}

void Application::InstallDefaultTerminateHandler() noexcept
{
    auto& application = Instance();
    if (application.m_terminateHandlerInstalled)
    {
        return;
    }

    std::set_terminate(DefaultTerminateHandler);
    application.m_terminateHandlerInstalled = true;
    if (application.m_logger)
    {
        application.m_logger->debug("Default terminate handler installed");
    }
}

void Application::Exit(int code)
{
    try
    {
        if (auto* logger = GetLogger())
        {
            logger->info("Exiting with code {}", code);
        }
    }
    catch (...)
    {
    }
    std::exit(code);
}

void Application::QuickExit(int code) noexcept
{
    try
    {
        if (auto* logger = GetLogger())
        {
            logger->warn("Quick exit requested with code {}", code);
        }
        auto& logManager = LogManager::Instance();
        if (logManager.IsInitialized())
        {
            logManager.Flush();
        }
    }
    catch (...)
    {
    }
    std::quick_exit(code);
}

void Application::Terminate() noexcept
{
    std::terminate();
}

void Application::Abort() noexcept
{
    try
    {
        if (auto* logger = GetLogger())
        {
            logger->critical("Abort requested");
            logger->flush();
        }
    }
    catch (...)
    {
    }
    std::abort();
}

void Application::InitializeCommandLine(int argc, char** argv)
{
    if (argc < 0 || (argc > 0 && argv == nullptr))
    {
        throw std::invalid_argument{"Invalid command-line arguments"};
    }

    int utf8Argc = argc;
    char** utf8Argv = argv;
    boost::nowide::args convertedArguments(utf8Argc, utf8Argv);

    auto& arguments = Instance().m_arguments;
    arguments.clear();
    arguments.reserve(static_cast<std::size_t>(utf8Argc));
    for (int index = 0; index < utf8Argc; ++index)
    {
        if (utf8Argv[index] == nullptr)
        {
            throw std::invalid_argument{"Command-line argument must not be null"};
        }
        arguments.emplace_back(utf8Argv[index]);
    }
}

void Application::ConfigureUtf8Locale()
{
#if defined(RAD_OS_WINDOWS)
    constexpr const char* candidates[] = {".UTF-8"};
#else
    constexpr const char* candidates[] = {"C.UTF-8", "en_US.UTF-8", "UTF-8", ""};
#endif

    const char* configuredLocale = nullptr;
    for (const char* candidate : candidates)
    {
        configuredLocale = std::setlocale(LC_ALL, candidate);
        if (configuredLocale != nullptr && IsUtf8Locale(configuredLocale))
        {
            break;
        }
        configuredLocale = nullptr;
    }
    if (configuredLocale == nullptr)
    {
        throw std::runtime_error{"No UTF-8 locale is available"};
    }

    const std::locale locale(configuredLocale);
    std::locale::global(locale);
    std::cin.imbue(locale);
    std::cout.imbue(locale);
    std::cerr.imbue(locale);
    std::clog.imbue(locale);
    static_cast<void>(boost::nowide::nowide_filesystem());
}

void Application::ConfigureUtf8Console()
{
#if defined(RAD_OS_WINDOWS)
    if (GetConsoleCP() != 0 && SetConsoleCP(CP_UTF8) == 0)
    {
        throw std::system_error(
            std::error_code(static_cast<int>(GetLastError()), std::system_category()),
            "SetConsoleCP");
    }
    if (GetConsoleOutputCP() != 0 && SetConsoleOutputCP(CP_UTF8) == 0)
    {
        throw std::system_error(
            std::error_code(static_cast<int>(GetLastError()), std::system_category()),
            "SetConsoleOutputCP");
    }
#endif
}

void Application::EnableDebugFeatures() noexcept
{
#if defined(_DEBUG)
    EnableMemoryLeakDetection();
#endif
}

} // namespace rad
