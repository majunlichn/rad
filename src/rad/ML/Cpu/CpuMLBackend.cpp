#include <rad/ML/Cpu/CpuMLBackend.h>

#include <rad/ML/Cpu/CpuMLDevice.h>

namespace rad
{

CpuMLBackend::CpuMLBackend()
{
    m_device = Ref<CpuMLDevice>(new CpuMLDevice(this));
}

MLDevice* CpuMLBackend::GetDevice(size_t index) noexcept
{
    return index == 0 ? m_device.get() : nullptr;
}

} // namespace rad
