#include <rad/System/OS.h>

#include <rad/IO/File.h>
#include <rad/IO/Logging.h>

#include <cstdlib>
#include <cstring>

#if defined(RAD_OS_WINDOWS)
#include <Windows.h>
#include <process.h>
#else
#include <pwd.h>
#include <unistd.h>
#endif

namespace rad
{

std::vector<std::string> get_exec_path()
{
    const char* envPath = std::getenv("PATH");
    if (!envPath)
    {
        return {};
    }

#if defined(RAD_OS_WINDOWS)
    return StrSplit(envPath, ";", true);
#else
    return StrSplit(envPath, ":", true);
#endif
}

std::string getlogin()
{
#if defined(RAD_OS_WINDOWS)
    std::wstring buffer(1024, 0);
    unsigned long count = static_cast<unsigned long>(buffer.size());
    if (::GetUserNameW(buffer.data(), &count))
    {
        // Strip trailing null characters:
        buffer.resize(count > 0 ? count - 1 : 0);
        return StrFromWide(buffer);
    }
    return {};
#else
    // POSIX: environment, password database, then controlling tty login name.
    if (const char* logname = std::getenv("LOGNAME"))
    {
        return logname;
    }
    if (const char* user = std::getenv("USER"))
    {
        return user;
    }
#if defined(RAD_OS_LINUX)
    if (const char* username = std::getenv("USERNAME"))
    {
        return username;
    }
#endif

    if (const passwd* pw = getpwuid(geteuid()); pw != nullptr && pw->pw_name != nullptr)
    {
        return pw->pw_name;
    }

#if defined(LOGIN_NAME_MAX)
    std::string name(LOGIN_NAME_MAX, '\0');
    if (getlogin_r(name.data(), name.size()) == 0)
    {
        name.resize(std::strlen(name.c_str()));
        return name;
    }
#else
    if (const char* ttyLogin = ::getlogin())
    {
        return ttyLogin;
    }
#endif

    return {};
#endif
}

int getpid()
{
#if defined(RAD_OS_WINDOWS)
    return ::_getpid();
#else
    return ::getpid();
#endif
}

} // namespace rad
