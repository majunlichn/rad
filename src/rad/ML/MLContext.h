#pragma once

#include <rad/ML/MLCommon.h>

#include <rad/Common/RefCounted.h>
#include <rad/Common/Span.h>

namespace rad
{

class MLDevice;
class MLOp;
class MLTensor;

// Per-device execution scope. Created by MLDevice. MLContext is not thread-safe; callers must
// externally synchronize concurrent use.
class MLContext : public RefCounted<MLContext>
{
public:
    MLContext(const MLContext&) = delete;
    MLContext& operator=(const MLContext&) = delete;

    [[nodiscard]] MLDevice* GetDevice() const noexcept { return m_device.get(); }

    void Execute(MLOp* op, Span<MLTensor* const> inputs, Span<MLTensor* const> outputs);
    virtual void Relu(MLTensor* input, MLTensor* output) = 0;
    virtual void LeakyRelu(MLTensor* input, MLTensor* output, float alpha = 0.01f) = 0;
    virtual void Sigmoid(MLTensor* input, MLTensor* output) = 0;
    virtual void Tanh(MLTensor* input, MLTensor* output) = 0;
    virtual void Gelu(MLTensor* input, MLTensor* output) = 0;
    virtual void Silu(MLTensor* input, MLTensor* output) = 0;
    virtual void Softplus(MLTensor* input, MLTensor* output) = 0;
    virtual void Elu(MLTensor* input, MLTensor* output, float alpha = 1.0f) = 0;
    virtual void Synchronize() = 0;

    virtual ~MLContext();

protected:
    explicit MLContext(Ref<MLDevice> device);

    Ref<MLDevice> m_device;
}; // class MLContext

} // namespace rad
