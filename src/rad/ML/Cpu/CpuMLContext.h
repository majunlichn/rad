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

    void Relu(MLTensor* input, MLTensor* output) override;
    void LeakyRelu(MLTensor* input, MLTensor* output, float alpha = 0.01f) override;
    void Sigmoid(MLTensor* input, MLTensor* output) override;
    void Tanh(MLTensor* input, MLTensor* output) override;
    void Gelu(MLTensor* input, MLTensor* output) override;
    void Silu(MLTensor* input, MLTensor* output) override;
    void Softplus(MLTensor* input, MLTensor* output) override;
    void Elu(MLTensor* input, MLTensor* output, float alpha = 1.0f) override;
    void Synchronize() override {}

private:
    void ExecuteUnaryOp(Ref<MLOp> op, MLTensor* input, MLTensor* output);

    Ref<MLOp> m_reluOp;
    Ref<MLOp> m_leakyReluOp;
    Ref<MLOp> m_sigmoidOp;
    Ref<MLOp> m_tanhOp;
    Ref<MLOp> m_geluOp;
    Ref<MLOp> m_siluOp;
    Ref<MLOp> m_softplusOp;
    Ref<MLOp> m_eluOp;
}; // class CpuMLContext

} // namespace rad
