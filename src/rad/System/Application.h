#pragma once

#include <rad/Common/Platform.h>
#include <rad/Common/RefCounted.h>
#include <rad/Common/String.h>
#include <rad/IO/File.h>
#include <rad/IO/Logging.h>

namespace rad
{

class Application : public RefCounted<Application>
{
public:
    Application();
    Application(int& argc, char**& argv);
    virtual ~Application();

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(Application&&) = delete;

    // Initializes the application:
    // - InitLogging.
    // - Converts the command line arguments to UTF-8.
    bool Init(int& argc, char**& argv);

    int GetArgc() const { return m_argc; }

    const std::vector<std::string>& GetArgv() const { return m_argv; }

    const char* GetArgv(size_t index) const
    {
        return (index < m_argv.size()) ? m_argv[index].c_str() : nullptr;
    }

    bool HasArg(std::string_view arg) const;

    std::string GetExecutablePath() const;
    std::string GetExecutableDirectory() const;
    std::string GetWorkingDirectory() const;
    bool SetWorkingDirectory(const std::string& path);

    // Causes normal program termination to occur.
    // Functions registered with std::atexit are called in the reverse order of their registration.
    void Exit(int code);
    // Causes normal program termination to occur without completely cleaning the resources.
    // Functions passed to std::at_quick_exit are called in reverse order of their registration.
    void QuickExit(int code);
    // In any case, std::terminate calls the currently installed std::terminate_handler.
    // The default std::terminate_handler calls std::abort.
    void Terminate();
    // Causes abnormal program termination (without cleaning up)
    // unless SIGABRT is being caught by a signal handler.
    void Abort();

private:
    bool InstallDefaultSignalHandlers();

    int m_argc = 0;
    std::vector<std::string> m_argv;

}; // Application

} // namespace rad
