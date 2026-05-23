#pragma once

#include <rad/ML/MLContext.h>

namespace rad
{

class CpuMLDevice;

class CpuMLContext final : public MLContext
{
public:
    explicit CpuMLContext(Ref<CpuMLDevice> device);

    [[nodiscard]] CpuMLDevice* GetCpuDevice() const noexcept;

    void Synchronize() override {}
}; // class CpuMLContext

} // namespace rad
