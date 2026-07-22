#include <rad/IO/Logging.h>

#include <spdlog/sinks/ostream_sink.h>

#include <gtest/gtest.h>

#include <sstream>
#include <string>

TEST(IO, Logging)
{
    auto& manager = rad::LogManager::Instance();
    manager.Shutdown();
    manager.Init();

    std::ostringstream output;
    manager.ClearSinks();
    manager.AddSink(std::make_shared<spdlog::sinks::ostream_sink_mt>(output));

    const auto logger = manager.CreateLogger("LoggingTest");
    logger->info("Hello, World!");
    logger->flush();
    EXPECT_NE(output.str().find("Hello, World!"), std::string::npos);

    manager.Shutdown();
}
