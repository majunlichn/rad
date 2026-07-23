#pragma once

#include <rad/Diagnostics/StackTrace.h>

#include <boost/exception/exception.hpp>

#include <cstddef>
#include <exception>
#include <source_location>
#include <string>
#include <type_traits>
#include <utility>

namespace rad
{

class Exception;

// Throws an Exception-derived object while recording the call site. Prefer this for derived
// exception types whose constructors do not accept and forward a std::source_location.
template <typename ExceptionType>
[[noreturn]] void ThrowException(
    ExceptionType exception, std::source_location location = std::source_location::current());

// Base class for failures specific to rad. Standard exception types should still be used when
// their semantics are a better match (for example, std::invalid_argument or std::system_error).
//
// The source location and stack trace are captured when the exception is constructed. Construct
// this class directly at the throw site. Derived types should either forward a source_location
// supplied by their constructor or be thrown through ThrowException.
class Exception : public virtual std::exception, public virtual boost::exception
{
public:
    explicit Exception(
        std::string message, std::source_location location = std::source_location::current(),
        std::size_t stackTraceDepth = 32);

    ~Exception() override = default;

    [[nodiscard]] const char* what() const noexcept override;
    [[nodiscard]] const std::string& Message() const noexcept;
    [[nodiscard]] const std::source_location& Location() const noexcept;
    [[nodiscard]] const boost::stacktrace::stacktrace& StackTrace() const noexcept;
    [[nodiscard]] std::string StackTraceString() const;

    // Produces a human-readable message containing the attached Boost error_info, source location,
    // and stack trace.
    [[nodiscard]] std::string DiagnosticInformation() const;

private:
    template <typename ExceptionType>
    friend void ThrowException(ExceptionType exception, std::source_location location);

    void SetLocation(std::source_location location);

    std::string m_message;
    std::source_location m_location;
    std::size_t m_stackTraceDepth;
    boost::stacktrace::stacktrace m_stackTrace;
};

template <typename ExceptionType>
[[noreturn]] void ThrowException(ExceptionType exception, std::source_location location)
{
    static_assert(std::is_base_of_v<Exception, ExceptionType>,
                  "ThrowException requires a type derived from rad::Exception");

    exception.SetLocation(location);
    exception.m_stackTrace = CaptureStackTrace(exception.m_stackTraceDepth, 1);
    throw std::move(exception);
}

} // namespace rad
