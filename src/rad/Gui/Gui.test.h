#pragma once

#include <rad/Gui/GuiApplication.h>
#include <rad/IO/Logging.h>

#include <gtest/gtest.h>

class TestEnvironment : public ::testing::Environment
{
public:
    TestEnvironment(int argc, char** argv);

    void SetUp() override;
    void TearDown() override;

    rad::GuiApplication* GetApp() { return m_app.get(); }

private:
    int m_argc = 0;
    char** m_argv = nullptr;
    rad::Ref<rad::GuiApplication> m_app;

}; // TestEnvironment

extern TestEnvironment* g_env;
