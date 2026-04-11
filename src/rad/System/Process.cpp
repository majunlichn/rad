#include <rad/System/Process.h>

#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <boost/process.hpp>

namespace rad
{

bool IsPipeReadNormally(const boost::system::error_code& ec)
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

std::future<std::string> Process::ExecuteAndCaptureOutput(const std::string& executable,
                                                          const std::vector<std::string>& args,
                                                          std::chrono::milliseconds timeout)
{
    namespace asio = boost::asio;
    namespace filesystem = boost::filesystem;
    namespace process = boost::process::v2;

    auto work = [executable, args, timeout]() -> std::string
    {
        asio::io_context io;
        asio::readable_pipe readPipe(io);

        auto env = boost::process::environment::current();

        filesystem::path exePath(StrToWide(executable));
        if (!exePath.has_parent_path() && !boost::filesystem::exists(exePath))
        {
            exePath = boost::process::environment::find_executable(exePath, env);
        }

        if (!boost::filesystem::exists(exePath))
        {
            return {};
        }

        process::process child(io, exePath, args, process::process_stdio{{}, readPipe, {}});

        // If a timeout is specified, set up a watchdog timer.
        const bool hasTimeout = (timeout != std::chrono::milliseconds::max());
        asio::steady_timer timer(io);
        if (hasTimeout)
        {
            timer.expires_after(timeout);
            timer.async_wait(
                [&child](const boost::system::error_code& ec)
                {
                    if (!ec)
                    {
                        boost::system::error_code killError;
                        child.terminate(killError);
                    }
                });
        }

        std::string output;
        asio::dynamic_string_buffer outputBuffer = asio::dynamic_buffer(output);

        boost::system::error_code readError;
        asio::async_read(readPipe, outputBuffer,
                         [&](const boost::system::error_code& ec, std::size_t /*bytes*/)
                         {
                             readError = ec;
                             if (hasTimeout)
                             {
                                 timer.cancel();
                             }
                         });
        io.run();

        if (!IsPipeReadNormally(readError))
        {
            throw std::runtime_error(readError.message());
        }

        // Wait for the process to exit and retrieve the exit code.
        boost::system::error_code waitError;
        int exitCode = child.wait(waitError);

        if (waitError && (waitError != boost::asio::error::eof))
        {
            throw std::runtime_error(waitError.message());
        }

        if (exitCode != 0)
        {
            throw std::runtime_error(std::format("Process exited abnormally ({}).", exitCode));
        }

        return output;
    };

    return std::async(std::launch::async, work);
}

} // namespace rad
