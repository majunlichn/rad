#pragma once

#include <rad/Common/RefCounted.h>

namespace rad
{

class MLContext;
class MLDevice;

// Abstract ML operation bound to a device (matmul, conv, custom kernel, ...).
class MLOp : public RefCounted<MLOp>
{
public:
    MLOp(const MLOp&) = delete;
    MLOp& operator=(const MLOp&) = delete;

    [[nodiscard]] MLDevice* GetDevice() const noexcept { return m_device.get(); }

    [[nodiscard]] virtual const char* GetName() const noexcept = 0;
    virtual void Execute(MLContext* context) = 0;

    virtual ~MLOp();

protected:
    explicit MLOp(Ref<MLDevice> device);

    Ref<MLDevice> m_device;
}; // class MLOp

} // namespace rad
