// Include this header before other headers to map CRT allocations in this translation unit.
#pragma once

#include <rad/Core/Platform.h>

#if defined(RAD_OS_WINDOWS) && defined(RAD_COMPILER_MSVC) && defined(_DEBUG)
// https://learn.microsoft.com/en-us/cpp/c-runtime-library/find-memory-leaks-using-the-crt-library
#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#define RAD_NEW new (_NORMAL_BLOCK, __FILE__, __LINE__)
#else
#define RAD_NEW new
#endif

namespace rad
{

// Enables automatic process-exit leak reporting. Call once during single-threaded startup.
// This function is a no-op outside MSVC Debug builds.
void EnableMemoryLeakDetection() noexcept;

// Captures the process-wide CRT debug heap state at construction.
class MemorySnapshot
{
public:
    MemorySnapshot() noexcept;
    ~MemorySnapshot() noexcept = default;

    // Reports the aggregate heap difference between two snapshots and returns whether it is
    // significant. Use only while unrelated allocating threads are quiescent.
    static bool Diff(const MemorySnapshot& before, const MemorySnapshot& after) noexcept;

private:
#if defined(RAD_OS_WINDOWS) && defined(RAD_COMPILER_MSVC) && defined(_DEBUG)
    _CrtMemState m_state = {};
#endif
}; // class MemorySnapshot

} // namespace rad
