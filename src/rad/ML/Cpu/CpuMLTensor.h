#pragma once

#include <rad/ML/MLTensor.h>

namespace rad
{

class CpuMLDevice;
class CpuMLBuffer;

class CpuMLTensor final : public MLTensor
{
public:
    CpuMLTensor(Ref<CpuMLDevice> device, const MLTensorDesc& desc, Ref<MLBuffer> buffer,
                size_t bufferOffset);

    [[nodiscard]] MLDevice* GetDevice() const noexcept override;
    [[nodiscard]] CpuMLDevice* GetCpuDevice() const noexcept { return m_device.get(); }
    [[nodiscard]] const MLTensorDesc& GetDesc() const noexcept override { return m_desc; }

    [[nodiscard]] MLBuffer* GetBuffer() const noexcept override { return m_buffer.get(); }
    [[nodiscard]] size_t GetBufferOffset() const noexcept override { return m_bufferOffset; }

private:
    Ref<CpuMLDevice> m_device;
    MLTensorDesc m_desc;
    Ref<MLBuffer> m_buffer;
    size_t m_bufferOffset = 0;
}; // class CpuMLTensor

} // namespace rad
