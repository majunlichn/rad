#pragma once

#include <rad/ML/MLDevice.h>

#include <rad/ML/Cpu/CpuMLBackend.h>

#include <cstdint>
#include <string_view>

namespace rad
{

class CpuMLContext;

class CpuMLDevice final : public MLDevice
{
public:
    [[nodiscard]] MLBackend* GetBackend() const noexcept override { return m_backend.get(); }
    [[nodiscard]] std::string_view GetName() const noexcept override;
    [[nodiscard]] uint32_t GetPciDeviceId() const noexcept override { return 0; }
    [[nodiscard]] bool IsDataTypeSupported(MLDataType dataType) const noexcept override;

    [[nodiscard]] Ref<MLBuffer> CreateBuffer(size_t sizeInBytes) override;
    [[nodiscard]] Ref<MLTensor> CreateTensor(const MLTensorDesc& desc, Ref<MLBuffer> buffer,
                                             size_t bufferOffset) override;
    [[nodiscard]] Ref<MLContext> CreateContext() override;

private:
    friend class CpuMLBackend;

    explicit CpuMLDevice(CpuMLBackend* backend);

    Ref<CpuMLBackend> m_backend;
}; // class CpuMLDevice

} // namespace rad
