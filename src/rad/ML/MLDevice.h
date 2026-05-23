#pragma once

#include <rad/ML/MLCommon.h>
#include <rad/ML/MLBuffer.h>

#include <rad/Common/RefCounted.h>

#include <cstdint>
#include <string_view>

namespace rad
{

class MLBackend;
class MLContext;
class MLTensor;

// Abstract compute device for ML workloads. Creates buffers, tensors, and contexts.
class MLDevice : public RefCounted<MLDevice>
{
public:
    MLDevice(const MLDevice&) = delete;
    MLDevice& operator=(const MLDevice&) = delete;

    [[nodiscard]] virtual MLBackend* GetBackend() const noexcept = 0;

    [[nodiscard]] virtual std::string_view GetName() const noexcept = 0;
    // PCI device identifier; 0 when not applicable (e.g. CPU).
    [[nodiscard]] virtual uint32_t GetPciDeviceId() const noexcept = 0;

    [[nodiscard]] virtual Ref<MLBuffer> CreateBuffer(size_t sizeInBytes) = 0;

    // Pass a null buffer to allocate storage sized for desc.
    [[nodiscard]] virtual Ref<MLTensor> CreateTensor(const MLTensorDesc& desc,
                                                     Ref<MLBuffer> buffer = nullptr,
                                                     size_t bufferOffset = 0) = 0;
    [[nodiscard]] virtual Ref<MLContext> CreateContext() = 0;

    virtual void WaitIdle() {}

    virtual ~MLDevice();

protected:
    MLDevice() = default;
}; // class MLDevice

} // namespace rad
