#include <rad/System/Application.h>

#include <gtest/gtest.h>

TEST(System, Application)
{
    EXPECT_FALSE(rad::Application::Arguments().empty());
    EXPECT_NO_THROW(rad::Application::InstallDefaultTerminateHandler());
    EXPECT_NO_THROW(rad::Application::InstallDefaultSignalHandlers());
}
