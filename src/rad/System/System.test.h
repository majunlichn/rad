#pragma once

#include <rad/System/Application.h>
#include <rad/IO/Logging.h>

#include <gtest/gtest.h>

class TestEnvironment : public ::testing::Environment
{
public:
    TestEnvironment(int argc, char** argv);

    void SetUp() override;
    void TearDown() override;

    rad::Application& GetApp() { return m_app; }

private:
    int m_argc = 0;
    char** m_argv = nullptr;
    rad::Application m_app;

}; // TestEnvironment

extern TestEnvironment* g_env;
