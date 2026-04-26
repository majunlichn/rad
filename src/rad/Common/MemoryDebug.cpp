#include <rad/Common/MemoryDebug.h>

#include <chrono>
#include <tuple>

namespace rad
{

void WorkaroundMemoryDebugFalsePositives()
{
#if defined(RAD_OS_WINDOWS) && defined(RAD_COMPILER_MSVC) && defined(_DEBUG)
    MemoryDebugIgnoreScope ignore;
    std::ignore = std::chrono::current_zone();
#endif
}

} // namespace rad
