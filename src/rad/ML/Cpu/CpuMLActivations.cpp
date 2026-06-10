#include <rad/ML/Cpu/CpuMLActivations.h>

namespace rad
{

Ref<MLOp> CpuMLCreateReluOp(Ref<CpuMLDevice> device)
{
    return CpuMLCreateElementWiseUnaryOp<CpuMLElementWiseUnaryFn::Relu>(std::move(device));
}

Ref<MLOp> CpuMLCreateLeakyReluOp(Ref<CpuMLDevice> device, float alpha)
{
    return CpuMLCreateElementWiseUnaryOp<CpuMLElementWiseUnaryFn::LeakyRelu>(
        std::move(device), CpuMLElementWiseUnaryFn::LeakyRelu{alpha});
}

Ref<MLOp> CpuMLCreateSigmoidOp(Ref<CpuMLDevice> device)
{
    return CpuMLCreateElementWiseUnaryOp<CpuMLElementWiseUnaryFn::Sigmoid>(std::move(device));
}

Ref<MLOp> CpuMLCreateTanhOp(Ref<CpuMLDevice> device)
{
    return CpuMLCreateElementWiseUnaryOp<CpuMLElementWiseUnaryFn::Tanh>(std::move(device));
}

Ref<MLOp> CpuMLCreateGeluOp(Ref<CpuMLDevice> device)
{
    return CpuMLCreateElementWiseUnaryOp<CpuMLElementWiseUnaryFn::Gelu>(std::move(device));
}

Ref<MLOp> CpuMLCreateFastGeluOp(Ref<CpuMLDevice> device)
{
    return CpuMLCreateElementWiseUnaryOp<CpuMLElementWiseUnaryFn::FastGelu>(std::move(device));
}

Ref<MLOp> CpuMLCreateSiluOp(Ref<CpuMLDevice> device)
{
    return CpuMLCreateElementWiseUnaryOp<CpuMLElementWiseUnaryFn::Silu>(std::move(device));
}

Ref<MLOp> CpuMLCreateSoftplusOp(Ref<CpuMLDevice> device)
{
    return CpuMLCreateElementWiseUnaryOp<CpuMLElementWiseUnaryFn::Softplus>(std::move(device));
}

Ref<MLOp> CpuMLCreateEluOp(Ref<CpuMLDevice> device, float alpha)
{
    return CpuMLCreateElementWiseUnaryOp<CpuMLElementWiseUnaryFn::Elu>(
        std::move(device), CpuMLElementWiseUnaryFn::Elu{alpha});
}

} // namespace rad
