#pragma once

#include <rad/Gui/GuiApplication.h>
#include <rad/Multimedia/MultimediaCommon.h>

#include <gtest/gtest.h>

class TestEnvironment : public ::testing::Environment
{
public:
    /// If argv contains a well-formed `--max-frames=N`, assigns N to m_maxFrames; otherwise leaves
    /// m_maxFrames at its initial value (-1).
    TestEnvironment(int argc, char** argv);

    void SetUp() override;
    void TearDown() override;

    rad::GuiApplication* GetApp() { return rad::GuiApplication::GetInstance(); }

private:
    int m_argc = 0;
    char** m_argv = nullptr;

}; // class TestEnvironment

extern TestEnvironment* g_env;
