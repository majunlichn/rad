#include <rad/ML/Cpu/CpuMLDevice.h>

#include <rad/System/CpuInfo.h>

#include <rad/ML/Cpu/CpuMLBackend.h>
#include <rad/ML/Cpu/CpuMLBuffer.h>
#include <rad/ML/Cpu/CpuMLContext.h>
#include <rad/ML/Cpu/CpuMLTensor.h>
#include <rad/ML/MLCommon.h>

#include <format>
#include <vector>

#include <stdexcept>

namespace rad
{

CpuMLDevice::CpuMLDevice(CpuMLBackend* backend) :
    m_backend(Ref<CpuMLBackend>(backend))
{
}

std::string_view CpuMLDevice::GetName() const noexcept
{
    return GetCpuBrandString();
}

bool CpuMLDevice::IsDataTypeSupported(MLDataType dataType) const noexcept
{
    switch (dataType)
    {
    case MLDataType::Float32:
    case MLDataType::Float64:
    case MLDataType::Int8:
    case MLDataType::Int16:
    case MLDataType::Int32:
    case MLDataType::Int64:
    case MLDataType::Uint8:
    case MLDataType::Uint16:
    case MLDataType::Uint32:
    case MLDataType::Uint64:
    case MLDataType::Bool:
    case MLDataType::Float16:
    case MLDataType::BFloat16:
    case MLDataType::Float8E4M3:
    case MLDataType::Float8E5M2:
        return true;
    case MLDataType::Undefined:
    default:
        return false;
    }
}

Ref<MLBuffer> CpuMLDevice::CreateBuffer(size_t sizeInBytes)
{
    if (sizeInBytes == 0)
    {
        throw std::invalid_argument("CpuMLDevice::CreateBuffer requires a non-zero size");
    }

    return Ref<CpuMLBuffer>(new CpuMLBuffer(this, std::vector<std::byte>(sizeInBytes)));
}

Ref<MLTensor> CpuMLDevice::CreateTensor(const MLTensorDesc& desc, Ref<MLBuffer> buffer,
                                        size_t bufferOffset)
{
    if (!ValidateTensorDesc(desc))
    {
        throw std::invalid_argument("CpuMLDevice::CreateTensor requires a valid tensor desc");
    }

    if (!IsDataTypeSupported(desc.dataType))
    {
        throw std::invalid_argument(
            std::format("CpuMLDevice::CreateTensor does not support tensor data type {}",
                        GetDataTypeName(desc.dataType)));
    }

    if (!buffer)
    {
        buffer = CreateBuffer(GetTensorStorageSize(desc));
    }
    else if (buffer->GetDevice() != this)
    {
        throw std::invalid_argument("CpuMLDevice::CreateTensor requires a buffer on this device");
    }

    return Ref<CpuMLTensor>(
        new CpuMLTensor(Ref<CpuMLDevice>(this), desc, std::move(buffer), bufferOffset));
}

Ref<MLContext> CpuMLDevice::CreateContext()
{
    return Ref<CpuMLContext>(new CpuMLContext(Ref<CpuMLDevice>(this)));
}

} // namespace rad
