#include <rad/Common/Memory.h>

#include <rad/Common/Integer.h>

#include <cassert>
#include <cstdlib>

#if defined(RAD_OS_WINDOWS)
#include <malloc.h> // _aligned_malloc/_aligned_free
#endif

namespace rad
{

void *AlignedAlloc(std::size_t size, std::size_t alignment)
{
    assert(IsPow2(alignment));
    // Prevent overflow when rounding size up.
    if (size > std::numeric_limits<std::size_t>::max() - (alignment - 1))
    {
        return nullptr;
    }
    // Round size up to a multiple of alignment.
    size = Pow2AlignUp(size, alignment);
#if defined(RAD_OS_WINDOWS)
    return _aligned_malloc(size, alignment);
#else
    return std::aligned_alloc(alignment, size);
#endif
}

void AlignedFree(void *p)
{
#if defined(RAD_OS_WINDOWS)
    _aligned_free(p);
#else
    std::free(p);
#endif
}

} // namespace rad
