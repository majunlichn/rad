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

} // namespace rad
