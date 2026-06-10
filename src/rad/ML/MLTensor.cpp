#include <rad/ML/MLTensor.h>

#include <rad/ML/MLBuffer.h>
#include <rad/ML/MLDevice.h>
#include <rad/ML/MLGlobal.h>

#include <stdexcept>

namespace rad
{

MLTensor::~MLTensor() = default;

size_t MLTensor::GetElementCount() const
{
    return GetTensorElementCount(GetDesc());
}

size_t MLTensor::GetBufferSize() const
{
    return GetTensorStorageSize(GetDesc());
}

bool MLTensor::IsContiguous() const noexcept
{
    return IsTensorContiguous(GetDesc());
}

void* MLTensor::MapMemory()
{
    MLBuffer* buffer = GetBuffer();
    if (!buffer)
    {
        throw std::invalid_argument("MLTensor::MapMemory requires a valid buffer");
    }

    return buffer->MapMemory(GetBufferOffset(), GetBufferSize());
}

void MLTensor::UnmapMemory() noexcept
{
    if (MLBuffer* buffer = GetBuffer())
    {
        buffer->UnmapMemory();
    }
}

Ref<MLTensor> MLCreateTensor(const MLTensorDesc& desc, std::string_view deviceId)
{
    MLDevice* device = GetMLDevice(deviceId);
    if (!device)
    {
        throw std::invalid_argument("MLCreateTensor requires a valid device ID");
    }

    return device->CreateTensor(desc);
}

Ref<MLTensor> MLCreateTensor(const MLTensorDesc& desc, Ref<MLBuffer> buffer, size_t bufferOffset)
{
    if (!buffer)
    {
        throw std::invalid_argument("MLCreateTensor requires a valid buffer");
    }

    MLDevice* device = buffer->GetDevice();
    if (!device)
    {
        throw std::invalid_argument("MLCreateTensor requires a buffer with a device");
    }

    return device->CreateTensor(desc, std::move(buffer), bufferOffset);
}

Ref<MLTensor> MLCreateTensor(Span<const size_t> shape, MLDataType dataType,
                             std::string_view deviceId)
{
    MLTensorDesc desc;
    desc.shape.assign(shape.begin(), shape.end());
    desc.dataType = dataType;
    return MLCreateTensor(desc, deviceId);
}

} // namespace rad
