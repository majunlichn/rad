#include <rad/IO/Logging.h>

#include <spdlog/cfg/env.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/msvc_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <vector>

namespace rad
{

static std::shared_ptr<spdlog::logger> g_logger;

spdlog::logger* GetDefaultLogger()
{
    return g_logger.get();
}

bool InitLogging(spdlog::sink_ptr fileSink)
{
    try
    {
        std::vector<spdlog::sink_ptr> sinks;
        sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());

        if (fileSink)
        {
            sinks.push_back(fileSink);
        }

#if defined(RAD_COMPILER_MSVC) && defined(_DEBUG)
        sinks.push_back(std::make_shared<spdlog::sinks::msvc_sink_mt>());
#endif

        // Clear any existing logger with the same name to prevent spdlog exceptions on re-init
        spdlog::drop("Default");

        g_logger = std::make_shared<spdlog::logger>("Default", sinks.begin(), sinks.end());

        spdlog::register_logger(g_logger);
        spdlog::set_default_logger(g_logger);

        // Custom formatting
        spdlog::set_pattern("%^[%T.%e] %n (%l)%$: %v");
        spdlog::flush_on(spdlog::level::warn);
        spdlog::cfg::load_env_levels();

        return g_logger != nullptr;
    }
    catch (const spdlog::spdlog_ex&)
    {
        return false;
    }
}

bool InitLogging(const std::string& filename, bool truncate)
{
    try
    {
        spdlog::sink_ptr fileSink =
            std::make_shared<spdlog::sinks::basic_file_sink_mt>(filename, truncate);
        return InitLogging(fileSink);
    }
    catch (const spdlog::spdlog_ex&)
    {
        // Happens if basic_file_sink_mt fails to open the file (bad path, permissions, etc).
        return false;
    }
}

std::shared_ptr<spdlog::logger> CreateLogger(const std::string& name)
{
    if (!g_logger)
    {
        // Ensure a fallback if the default logger wasn't initialized first
        return nullptr;
    }

    auto& sinks = g_logger->sinks();
    std::shared_ptr<spdlog::logger> logger =
        std::make_shared<spdlog::logger>(name, sinks.begin(), sinks.end());
    spdlog::initialize_logger(logger);
    return logger;
}

} // namespace rad
