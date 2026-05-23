#pragma once

#include <rad/ML/MLCommon.h>

#include <rad/Common/RefCounted.h>

namespace rad
{

class MLDevice;
class MLOp;

// Per-device execution scope. Created by MLDevice.
class MLContext : public RefCounted<MLContext>
{
public:
    MLContext(const MLContext&) = delete;
    MLContext& operator=(const MLContext&) = delete;

    [[nodiscard]] MLDevice* GetDevice() const noexcept { return m_device.get(); }

    void Execute(MLOp* op);
    virtual void Synchronize() = 0;

    virtual ~MLContext();

protected:
    explicit MLContext(Ref<MLDevice> device);

    Ref<MLDevice> m_device;
}; // class MLContext

} // namespace rad
