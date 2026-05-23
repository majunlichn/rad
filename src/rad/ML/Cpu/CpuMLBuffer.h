#pragma once

#include <rad/ML/MLBuffer.h>

#include <vector>

namespace rad
{

class CpuMLDevice;

class CpuMLBuffer final : public MLBuffer
{
public:
    [[nodiscard]] MLDevice* GetDevice() const noexcept override;
    [[nodiscard]] size_t GetSize() const noexcept override { return m_storage.size(); }

    [[nodiscard]] void* MapMemory(size_t offset = 0, size_t size = 0) override;
    void UnmapMemory() noexcept override;

private:
    friend class CpuMLDevice;

    explicit CpuMLBuffer(CpuMLDevice* device, std::vector<std::byte> storage);

    CpuMLDevice* m_device = nullptr;
    std::vector<std::byte> m_storage;
}; // class CpuMLBuffer

} // namespace rad
