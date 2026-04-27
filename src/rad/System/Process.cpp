#include <rad/System/Process.h>

#include <rad/Common/UTFConv.h>

#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <boost/process.hpp>
#include <boost/process/v2/environment.hpp>

namespace rad
{

static bool IsPipeReadNormally(const boost::system::error_code& ec)
{
    // No error or normal EOF.
    if (!ec || ec == boost::asio::error::eof)
    {
        return true;
    }
#if defined(RAD_OS_WINDOWS)
    // ERROR_BROKEN_PIPE (109): "The pipe has been ended."
    else if ((ec.category() == boost::system::system_category()) &&
             (ec.value() == ERROR_BROKEN_PIPE))
    {
        return true;
    }
#endif
    return false;
}

std::string Process::ExecuteAndCaptureOutput(const std::string& executable,
                                             const std::vector<std::string>& args,
                                             std::optional<std::chrono::milliseconds> timeout)
{
    namespace asio = boost::asio;
    namespace filesystem = boost::filesystem;
    namespace process = boost::process::v2;

    asio::io_context io;
    asio::readable_pipe readPipe(io);

    auto env = boost::process::environment::current();

    filesystem::path exePath(UTFConv::ToWide(executable));
    if (!exePath.has_parent_path() && !boost::filesystem::exists(exePath))
    {
        exePath = boost::process::environment::find_executable(exePath, env);
    }
    else
    {
        if (!exePath.is_absolute())
        {
            exePath = filesystem::absolute(exePath);
        }
    }

    if (exePath.empty() || !boost::filesystem::exists(exePath))
    {
        return {};
    }

    process::process child(io, exePath, args, process::process_stdio{{}, readPipe, {}});

    std::string output;
    boost::system::error_code readError;
    boost::system::error_code timerError;
    bool isTimedOut = false;

    // If a timeout is specified, set up a watchdog timer.
    std::unique_ptr<asio::steady_timer> timer;
    if (timeout.has_value() && (timeout.value() > std::chrono::milliseconds::zero()))
    {
        timer = std::make_unique<asio::steady_timer>(io);
        timer->expires_after(timeout.value());
        timer->async_wait(
            [&](const boost::system::error_code& ec)
            {
                timerError = ec;
                if (!ec)
                {
                    isTimedOut = true;
                    boost::system::error_code killError;
                    child.terminate(killError);
                }
            });
    }

    asio::async_read(readPipe, asio::dynamic_buffer(output),
                     [&](const boost::system::error_code& ec, std::size_t /*bytes transferred*/)
                     {
                         readError = ec;
                         isTimedOut = false;
                         if (timer)
                         {
                             timer->cancel();
                         }
                     });
    io.run();

    if (isTimedOut)
    {
        if (child.running())
        {
            boost::system::error_code killError;
            child.terminate(killError);
        }
        boost::system::error_code waitError;
        child.wait(waitError);
        throw std::runtime_error(
            std::format("{}: timed out after {}ms.", executable, timeout.value().count()));
    }

    if (!IsPipeReadNormally(readError))
    {
        if (child.running())
        {
            boost::system::error_code killError;
            child.terminate(killError);
            boost::system::error_code waitError;
            child.wait(waitError);
        }
        throw std::runtime_error(std::format("{}: {}.", executable, readError.message()));
    }

    // Wait for the process to exit and retrieve the exit code.
    boost::system::error_code waitError;
    int exitCode = child.wait(waitError);

    if (waitError)
    {
        throw std::runtime_error(std::format("{}: {}.", executable, waitError.message()));
    }

    if (exitCode != 0)
    {
        throw std::runtime_error(
            std::format("{}: exited abnormally (code={}).", executable, exitCode));
    }

    return output;
}

} // namespace rad
