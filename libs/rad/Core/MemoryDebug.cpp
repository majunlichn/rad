#include <rad/Core/MemoryDebug.h>

#include <chrono>
#include <tuple>

namespace rad
{

void EnableMemoryLeakDetection() noexcept
{
#if defined(RAD_OS_WINDOWS) && defined(RAD_COMPILER_MSVC) && defined(_DEBUG)
    int oldFlags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
    _CrtSetDbgFlag(oldFlags & ~_CRTDBG_ALLOC_MEM_DF);
    // Workaround false positives:
    try
    {
        std::ignore = std::chrono::current_zone();
    }
    catch (...)
    {
        // Ignore failures
    }
    _CrtSetDbgFlag(oldFlags);

    // Track subsequent allocations and dump surviving blocks automatically at process exit.
    _CrtSetDbgFlag(oldFlags | _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
}

MemorySnapshot::MemorySnapshot() noexcept
{
#if defined(RAD_OS_WINDOWS) && defined(RAD_COMPILER_MSVC) && defined(_DEBUG)
    _CrtMemCheckpoint(&m_state);
#endif
}

bool MemorySnapshot::Diff(const MemorySnapshot& before, const MemorySnapshot& after) noexcept
{
#if defined(RAD_OS_WINDOWS) && defined(RAD_COMPILER_MSVC) && defined(_DEBUG)
    _CrtMemState diff = {};

    if (_CrtMemDifference(&diff, &before.m_state, &after.m_state))
    {
        _CrtMemDumpStatistics(&diff);
        return true;
    }
#endif

    return false;
}

} // namespace rad
