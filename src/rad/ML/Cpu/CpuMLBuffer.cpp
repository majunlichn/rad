#include <rad/ML/Cpu/CpuMLBuffer.h>

#include <rad/ML/Cpu/CpuMLDevice.h>

#include <stdexcept>

namespace rad
{

CpuMLBuffer::CpuMLBuffer(CpuMLDevice* device, std::vector<std::byte> storage) :
    m_device(device),
    m_storage(std::move(storage))
{
}

MLDevice* CpuMLBuffer::GetDevice() const noexcept
{
    return m_device;
}

void* CpuMLBuffer::MapMemory(size_t offset, size_t size)
{
    const size_t bufferSize = m_storage.size();
    if (offset > bufferSize)
    {
        throw std::out_of_range("CpuMLBuffer::MapMemory offset exceeds buffer size");
    }

    const size_t available = bufferSize - offset;
    if (size == 0)
    {
        size = available;
    }
    else if (size > available)
    {
        throw std::out_of_range("CpuMLBuffer::MapMemory size exceeds mapped range");
    }

    return static_cast<void*>(m_storage.data() + offset);
}

void CpuMLBuffer::UnmapMemory() noexcept
{
}

} // namespace rad
