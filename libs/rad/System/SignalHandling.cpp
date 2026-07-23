#include <rad/System/SignalHandling.h>

// Crash-context handling follows backward-cpp's platform-specific implementation:
// https://github.com/bombela/backward-cpp/blob/0bfd0a07a61551413ccd2ab9a9099af3bad40681/backward.hpp
#include <backward.hpp>

namespace rad
{

class SignalHandling::Impl
{
public:
    backward::SignalHandling handler;
};

SignalHandling::SignalHandling() : m_impl(std::make_unique<Impl>())
{
}

SignalHandling::~SignalHandling() = default;

bool SignalHandling::IsLoaded() const noexcept
{
    return m_impl->handler.loaded();
}

} // namespace rad
