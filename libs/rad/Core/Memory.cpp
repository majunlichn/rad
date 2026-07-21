#include <rad/Core/Memory.h>

#include <rad/Core/Integer.h>
#include <rad/Core/Platform.h>

#if defined(RAD_OS_WINDOWS)
#include <malloc.h>
#else
#include <cstdlib>
#endif

namespace rad
{

void* AlignedAlloc(std::size_t size, std::size_t alignment) noexcept
{
    if (size == 0 || !IsPowerOfTwo(alignment))
    {
        return nullptr;
    }

#if defined(RAD_OS_WINDOWS)
    return _aligned_malloc(size, alignment);
#else
    if (alignment < sizeof(void*))
    {
        alignment = sizeof(void*);
    }

    void* ptr = nullptr;
    return posix_memalign(&ptr, alignment, size) == 0 ? ptr : nullptr;
#endif
}

void AlignedFree(void* ptr) noexcept
{
#if defined(RAD_OS_WINDOWS)
    _aligned_free(ptr);
#else
    std::free(ptr);
#endif
}

} // namespace rad
