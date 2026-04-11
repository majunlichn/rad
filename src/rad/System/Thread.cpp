#include <rad/Common/String.h>
#include <rad/System/Thread.h>

#include <algorithm>
#include <array>
#include <cstring>

#if defined(RAD_OS_WINDOWS)
#include <Windows.h>
#else
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#if defined(__GLIBC__) && !defined(__APPLE__) && !defined(__ANDROID__)
#if __GLIBC_PREREQ(2, 12)
#define HAS_PTHREAD_SETNAME_NP 1
#endif
#endif

#if defined(HAS_PTHREAD_SETNAME_NP)
#define MAX_THREAD_NAME_LEN size_t(15)
#endif

namespace rad
{

bool SetThreadName(cstring_view name)
{
#if defined(RAD_OS_WINDOWS)
    std::wstring nameWide = rad::StrToWide(name);
    HRESULT hr = ::SetThreadDescription(::GetCurrentThread(), nameWide.c_str());
    return SUCCEEDED(hr);
#elif defined(HAS_PTHREAD_SETNAME_NP)
    std::array<char, MAX_THREAD_NAME_LEN + 1> buffer = {};
    size_t len = std::min<size_t>(name.size(), MAX_THREAD_NAME_LEN);
    std::memcpy(buffer.data(), name.c_str(), len);
    int err = pthread_setname_np(pthread_self(), buffer.data());
    return (err == 0);
#else
    return false;
#endif
}

std::string GetThreadName()
{
#if defined(_WIN32)
    PWSTR ppszThreadDescription = nullptr;
    HRESULT hr = ::GetThreadDescription(::GetCurrentThread(), &ppszThreadDescription);
    if (SUCCEEDED(hr))
    {
        std::wstring nameWide = ppszThreadDescription;
        LocalFree(ppszThreadDescription);
        ppszThreadDescription = nullptr;
        return StrFromWide(nameWide);
    }
    else
    {
        return {};
    }
#elif defined(HAS_PTHREAD_SETNAME_NP)
    std::string name(MAX_THREAD_NAME_LEN + 1, '\0');
    pthread_getname_np(pthread_self(), name.data(), name.size());
    name.resize(std::strlen(name.c_str()));
    return name;
#else
    return {};
#endif
}

uint64_t GetCurrentThreadId()
{
#if defined(RAD_OS_WINDOWS)
    return ::GetCurrentThreadId();
#elif defined(RAD_OS_LINUX) || defined(RAD_OS_ANDROID)
    return ::gettid();
#else
    uint64_t tid = 0;
    pthread_threadid_np(pthread_self(), &tid);
    return tid;
#endif
}

} // namespace rad
