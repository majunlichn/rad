#pragma once

#include <gtest/gtest.h>

class TestEnvironment : public ::testing::Environment
{
public:
    TestEnvironment(int argc, char** argv);

    void SetUp() override;
    void TearDown() override;

private:

}; // TestEnvironment

extern TestEnvironment* g_env;
