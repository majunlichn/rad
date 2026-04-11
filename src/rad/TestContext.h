#pragma once

#include <rad/System/Application.h>
#include <rad/IO/Logging.h>

#include <gtest/gtest.h>

class TestContext : public ::testing::Environment
{
public:
    int m_argc = 0;
    char** m_argv = nullptr;
    rad::Application m_app;

    TestContext(int argc, char** argv) :
        m_argc(argc),
        m_argv(argv)
    {
    }

    void SetUp() override { m_app.Init(m_argc, m_argv); }

    void TearDown() override {}

}; // TestContext

extern TestContext* g_context;
