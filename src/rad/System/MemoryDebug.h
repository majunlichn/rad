#pragma once

#if defined(_DEBUG)

#if defined(_WIN32)
// https://learn.microsoft.com/en-us/cpp/c-runtime-library/find-memory-leaks-using-the-crt-library
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define RAD_NEW new (_NORMAL_BLOCK, __FILE__, __LINE__)
#else
#define RAD_NEW new
#endif

#else // for release build
#define RAD_NEW new
#endif

#include <rad/Common/Platform.h>

namespace rad
{

#if defined(RAD_OS_WINDOWS) && defined(RAD_COMPILER_MSVC) && defined(_DEBUG)

class MemoryDebugIgnoreScope
{
public:
    MemoryDebugIgnoreScope()
    {
        m_old_flags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
        _CrtSetDbgFlag(m_old_flags & ~_CRTDBG_ALLOC_MEM_DF);
    }

    ~MemoryDebugIgnoreScope() { _CrtSetDbgFlag(m_old_flags); }

private:
    int m_old_flags;

}; // MemoryIgnoreBlock

#else

class MemoryDebugIgnoreScope
{
public:
    MemoryDebugIgnoreScope() = default;
    ~MemoryDebugIgnoreScope() = default;
};

#endif

// Internal allocations may be marked as normal blocks instead of CRT blocks or client blocks.
void WorkaroundMemoryDebugFalsePositives();

} // namespace rad
