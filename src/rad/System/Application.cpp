#include <rad/Common/MemoryDebug.h> // must be included before the other headers.

#include <rad/System/Application.h>

#include <rad/IO/Logging.h>
#include <rad/System/CpuInfo.h>
#include <rad/System/OS.h>

#include <algorithm>
#include <chrono>
#include <tuple>

#include <boost/nowide/args.hpp>
#include <boost/nowide/cstdlib.hpp>
#include <boost/nowide/filesystem.hpp>

#include <backward.hpp>

#if defined(RAD_OS_WINDOWS)
#include <Windows.h>
#else
#include <unistd.h>
#endif

namespace rad
{

Application::Application()
{
    InstallDefaultSignalHandlers();
}

Application::Application(int& argc, char**& argv) :
    Application()
{
    if (!Init(argc, argv))
    {
        SPDLOG_ERROR("Application::Init failed!");
    }
}

Application::~Application()
{
}

bool Application::Init(int& argc, char**& argv)
{
#if defined(RAD_OS_WINDOWS)
#if defined(_DEBUG)
    WorkaroundMemoryDebugFalsePositives();
    ::_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
    boost::nowide::args nowideArgs(argc, argv);

    std::setlocale(LC_ALL, ".UTF-8");
    ::SetConsoleCP(CP_UTF8);
    ::SetConsoleOutputCP(CP_UTF8);
#endif
    boost::nowide::nowide_filesystem();

    std::string commandLine;
    m_argc = argc;
    m_argv.reserve(argc);
    for (int i = 0; i < argc; ++i)
    {
        std::string arg = StrTrim(argv[i]);
        m_argv.push_back(arg);
        commandLine += arg;
        if (i < argc - 1)
        {
            commandLine += " ";
        }
    }

    std::string logFileName = pystring::os::path::basename(argv[0]) + ".log";
    InitLogging(logFileName, true);

    SPDLOG_INFO("Executable: {}", GetExecutablePath());
    SPDLOG_INFO("Command: {}", commandLine);
    SPDLOG_INFO("Working Directory: {}", GetWorkingDirectory());

#if defined(CPU_FEATURES_ARCH_X86)
    const auto& cpuInfo = GetX86Info();
    SPDLOG_INFO("CPU: {} ({})", StrTrim(cpuInfo.brand_string), StrTrim(cpuInfo.vendor));
#endif

    return true;
}

bool Application::HasArg(std::string_view arg) const
{
    return std::find(m_argv.begin(), m_argv.end(), arg) != m_argv.end();
}

std::string Application::GetExecutablePath() const
{
#if defined(RAD_OS_WINDOWS)
    DWORD bufferSize = MAX_PATH;
    std::vector<wchar_t> buffer(bufferSize);
    DWORD length = GetModuleFileNameW(nullptr, buffer.data(), bufferSize);
    while ((length == bufferSize) && (GetLastError() == ERROR_INSUFFICIENT_BUFFER))
    {
        bufferSize *= 2;
        buffer.resize(bufferSize);
        length = GetModuleFileNameW(nullptr, buffer.data(), bufferSize);
    }
    if (length > 0)
    {
        return WideToMultiByte(std::wstring(buffer.data(), length));
    }
    else
    {
        return {};
    }
#elif defined(RAD_OS_LINUX) || defined(RAD_OS_ANDROID)
    char buffer[4096];
    ssize_t length = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (length != -1)
    {
        buffer[length] = '\0';
        return std::string(buffer);
    }
    return {};
#else
    std::string pathString;
    if (m_argv[0].empty())
    {
        return {};
    }
    std::filesystem::path path((const char8_t*)m_argv[0].c_str());
    std::error_code ec;
    if (path.is_absolute())
    {
        pathString = (const char*)std::filesystem::canonical(path, ec).u8string().c_str();
    }
    else
    {
        std::error_code ec;
        std::filesystem::path currentPath = std::filesystem::current_path(ec);
        if (!ec)
        {
            path = std::filesystem::absolute(currentPath / path, ec);
            if (!ec)
            {
                pathString = (const char*)path.u8string().c_str();
            }
        }
    }
    return pathString;
#endif
}

std::string Application::GetExecutableDirectory() const
{
    return pystring::os::path::dirname(GetExecutablePath());
}

std::string Application::GetWorkingDirectory() const
{
    std::error_code ec;
    std::string workingDir = (const char*)std::filesystem::current_path(ec).u8string().c_str();
    if (!ec)
    {
        return workingDir;
    }
    else
    {
        return {};
    }
}

bool Application::SetWorkingDirectory(const std::string& path)
{
    std::error_code ec;
    std::filesystem::current_path((const char8_t*)path.c_str(), ec);
    return !ec;
}

void Application::Exit(int code)
{
    std::exit(code);
}

void Application::QuickExit(int code)
{
    std::quick_exit(code);
}

void Application::Terminate()
{
    std::terminate();
}

void Application::Abort()
{
    std::abort();
}

bool Application::InstallDefaultSignalHandlers()
{
    static backward::SignalHandling signalHandling;
    return signalHandling.loaded();
}

} // namespace rad
