#pragma once

#include <rad/ML/MLBackend.h>

namespace rad
{

class CpuMLDevice;

class CpuMLBackend final : public MLBackend
{
public:
    CpuMLBackend();

    [[nodiscard]] std::string_view GetName() const noexcept override { return m_name; }

    [[nodiscard]] size_t GetDeviceCount() const override { return 1; }
    [[nodiscard]] MLDevice* GetDevice(size_t index) noexcept override;

    [[nodiscard]] CpuMLDevice* GetCpuDevice() noexcept { return m_device.get(); }

private:
    static constexpr std::string_view m_name = "cpu";

    Ref<CpuMLDevice> m_device;
}; // class CpuMLBackend

} // namespace rad
