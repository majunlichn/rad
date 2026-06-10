#pragma once

#include <rad/Common/RefCounted.h>
#include <rad/Common/Span.h>

#include <cstddef>
#include <type_traits>

namespace rad
{

class MLContext;
class MLDevice;
class MLTensor;

// Abstract ML operation bound to a device (matmul, conv, custom kernel, ...).
class MLOp : public RefCounted<MLOp>
{
public:
    MLOp(const MLOp&) = delete;
    MLOp& operator=(const MLOp&) = delete;

    [[nodiscard]] MLDevice* GetDevice() const noexcept { return m_device.get(); }

    [[nodiscard]] virtual const char* GetName() const noexcept = 0;
    virtual void SetParameters(const void* data, size_t dataSize);

    template <typename T>
        requires std::is_trivially_copyable_v<T>
    void SetParameters(const T& parameters)
    {
        SetParameters(&parameters, sizeof(T));
    }

    virtual void Execute(MLContext* context, Span<MLTensor* const> inputs,
                         Span<MLTensor* const> outputs) = 0;

    virtual ~MLOp();

protected:
    explicit MLOp(Ref<MLDevice> device);

    Ref<MLDevice> m_device;
}; // class MLOp

} // namespace rad
