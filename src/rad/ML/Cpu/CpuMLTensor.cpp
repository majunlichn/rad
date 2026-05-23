#include <rad/ML/Cpu/CpuMLTensor.h>

#include <rad/ML/Cpu/CpuMLDevice.h>
#include <rad/ML/MLBuffer.h>
#include <rad/ML/MLCommon.h>

#include <cassert>
#include <stdexcept>

namespace rad
{

CpuMLTensor::CpuMLTensor(Ref<CpuMLDevice> device, const MLTensorDesc& desc, Ref<MLBuffer> buffer,
                         size_t bufferOffset) :
    m_device(std::move(device)),
    m_desc(desc),
    m_buffer(std::move(buffer)),
    m_bufferOffset(bufferOffset)
{
    if (!m_buffer)
    {
        throw std::invalid_argument("CpuMLTensor requires a valid buffer");
    }

    assert(m_device.get() == m_buffer->GetDevice());

    const size_t sizeInBytes = GetTensorStorageSize(desc);
    const size_t bufferSize = m_buffer->GetSize();
    if (bufferOffset > bufferSize || sizeInBytes > bufferSize - bufferOffset)
    {
        throw std::out_of_range("CpuMLTensor view exceeds buffer storage");
    }
}

MLDevice* CpuMLTensor::GetDevice() const noexcept
{
    return m_device.get();
}

} // namespace rad
