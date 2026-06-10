#include <rad/ML/Cpu/CpuMLContext.h>

#include <rad/ML/Cpu/CpuMLActivations.h>
#include <rad/ML/Cpu/CpuMLDevice.h>
#include <rad/ML/MLOp.h>
#include <rad/ML/MLTensor.h>

namespace rad
{

CpuMLContext::CpuMLContext(Ref<CpuMLDevice> device) :
    MLContext(device),
    m_reluOp(CpuMLCreateReluOp(device)),
    m_leakyReluOp(CpuMLCreateLeakyReluOp(device)),
    m_sigmoidOp(CpuMLCreateSigmoidOp(device)),
    m_tanhOp(CpuMLCreateTanhOp(device)),
    m_geluOp(CpuMLCreateGeluOp(device)),
    m_siluOp(CpuMLCreateSiluOp(device)),
    m_softplusOp(CpuMLCreateSoftplusOp(device)),
    m_eluOp(CpuMLCreateEluOp(device))
{
}

CpuMLDevice* CpuMLContext::GetCpuDevice() const noexcept
{
    return static_cast<CpuMLDevice*>(GetDevice());
}

void CpuMLContext::ExecuteUnaryOp(Ref<MLOp> op, MLTensor* input, MLTensor* output)
{
    Execute(op.get(), input, output);
}

void CpuMLContext::Relu(MLTensor* input, MLTensor* output)
{
    ExecuteUnaryOp(m_reluOp, input, output);
}

void CpuMLContext::LeakyRelu(MLTensor* input, MLTensor* output, float alpha)
{
    m_leakyReluOp->SetParameters(CpuMLElementWiseUnaryFn::LeakyRelu::Parameters{alpha});
    ExecuteUnaryOp(m_leakyReluOp, input, output);
}

void CpuMLContext::Sigmoid(MLTensor* input, MLTensor* output)
{
    ExecuteUnaryOp(m_sigmoidOp, input, output);
}

void CpuMLContext::Tanh(MLTensor* input, MLTensor* output)
{
    ExecuteUnaryOp(m_tanhOp, input, output);
}

void CpuMLContext::Gelu(MLTensor* input, MLTensor* output)
{
    ExecuteUnaryOp(m_geluOp, input, output);
}

void CpuMLContext::Silu(MLTensor* input, MLTensor* output)
{
    ExecuteUnaryOp(m_siluOp, input, output);
}

void CpuMLContext::Softplus(MLTensor* input, MLTensor* output)
{
    ExecuteUnaryOp(m_softplusOp, input, output);
}

void CpuMLContext::Elu(MLTensor* input, MLTensor* output, float alpha)
{
    m_eluOp->SetParameters(CpuMLElementWiseUnaryFn::Elu::Parameters{alpha});
    ExecuteUnaryOp(m_eluOp, input, output);
}

} // namespace rad
