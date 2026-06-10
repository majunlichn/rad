#include <rad/ML/MLOp.h>

#include <rad/ML/MLDevice.h>

#include <stdexcept>

namespace rad
{

MLOp::MLOp(Ref<MLDevice> device) :
    m_device(std::move(device))
{
    if (!m_device)
    {
        throw std::invalid_argument("MLOp requires a valid device");
    }
}

MLOp::~MLOp() = default;

void MLOp::SetParameters(const void* data, size_t dataSize)
{
    (void)data;

    if (dataSize != 0)
    {
        throw std::invalid_argument("MLOp does not accept parameters");
    }
}

} // namespace rad
