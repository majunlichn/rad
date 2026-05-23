#pragma once

#include <rad/Common/RefCounted.h>

#include <cstddef>

namespace rad
{

class MLDevice;

// Owns byte storage on an ML device.
class MLBuffer : public RefCounted<MLBuffer>
{
public:
    MLBuffer(const MLBuffer&) = delete;
    MLBuffer& operator=(const MLBuffer&) = delete;

    [[nodiscard]] virtual MLDevice* GetDevice() const noexcept = 0;
    [[nodiscard]] virtual size_t GetSize() const noexcept = 0; // Total allocated bytes.

    // Maps buffer bytes for host access. size == 0 maps from offset through the end.
    [[nodiscard]] virtual void* MapMemory(size_t offset = 0, size_t size = 0) = 0;
    virtual void UnmapMemory() noexcept = 0;

    virtual ~MLBuffer();

protected:
    MLBuffer() = default;
}; // class MLBuffer

} // namespace rad
