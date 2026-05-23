#include <rad/ML/Cpu/CpuMLContext.h>

#include <rad/ML/Cpu/CpuMLDevice.h>

namespace rad
{

CpuMLContext::CpuMLContext(Ref<CpuMLDevice> device) :
    MLContext(device)
{
}

CpuMLDevice* CpuMLContext::GetCpuDevice() const noexcept
{
    return static_cast<CpuMLDevice*>(GetDevice());
}

} // namespace rad
