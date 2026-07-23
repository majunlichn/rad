#pragma once

#include <memory>

namespace rad
{

// Installs cross-platform fatal-signal and unhandled-exception diagnostics.
class SignalHandling final
{
public:
    SignalHandling();
    ~SignalHandling();

    SignalHandling(const SignalHandling&) = delete;
    SignalHandling& operator=(const SignalHandling&) = delete;
    SignalHandling(SignalHandling&&) = delete;
    SignalHandling& operator=(SignalHandling&&) = delete;

    [[nodiscard]] bool IsLoaded() const noexcept;

private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
}; // class SignalHandling

} // namespace rad
