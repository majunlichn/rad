#include <rad/ML/MLContext.h>

#include <rad/ML/MLDevice.h>
#include <rad/ML/MLOp.h>

namespace rad
{

MLContext::MLContext(Ref<MLDevice> device) :
    m_device(std::move(device))
{
}

MLContext::~MLContext() = default;

void MLContext::Execute(MLOp* op)
{
    if (op)
    {
        op->Execute(this);
    }
}

} // namespace rad
