#include <rad/Common/Platform.h>
#include <iostream>

#include <gtest/gtest.h>

TEST(Common, Platform)
{
#if defined(RAD_OS_WINDOWS) && defined(RAD_COMPILER_MSVC)
    std::cout << "Compiled on Windows use Microsoft C++ (MSVC) compiler." << std::endl;
#endif
}
