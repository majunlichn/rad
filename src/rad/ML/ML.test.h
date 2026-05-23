#pragma once

#include <gtest/gtest.h>

class MLTestEnvironment : public ::testing::Environment
{
public:
    void SetUp() override;
    void TearDown() override;
}; // class MLTestEnvironment

extern MLTestEnvironment* g_env;
