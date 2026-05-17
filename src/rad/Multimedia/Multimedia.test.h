#pragma once

#include <rad/Multimedia/MultimediaCommon.h>

#include <gtest/gtest.h>

class TestEnvironment : public ::testing::Environment
{
public:
    TestEnvironment(int argc, char** argv);
    void SetUp() override;
    void TearDown() override;

private:
    int m_argc;
    char** m_argv;
}; // class TestEnvironment
