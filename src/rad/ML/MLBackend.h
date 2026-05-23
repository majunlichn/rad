#pragma once

#include <rad/ML/MLCommon.h>

#include <rad/Common/RefCounted.h>

#include <cstddef>
#include <string_view>

namespace rad
{

class MLDevice;

// Owns and enumerates devices for a single backend (CPU, Vulkan, ...).
class MLBackend : public RefCounted<MLBackend>
{
public:
    MLBackend(const MLBackend&) = delete;
    MLBackend& operator=(const MLBackend&) = delete;

    [[nodiscard]] virtual std::string_view GetName() const noexcept = 0;
    [[nodiscard]] virtual size_t GetDeviceCount() const = 0;
    [[nodiscard]] virtual MLDevice* GetDevice(size_t index) noexcept = 0;

    virtual ~MLBackend();

protected:
    MLBackend() = default;
}; // class MLBackend

} // namespace rad
