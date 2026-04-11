#include <rad/System/Application.h>

#include <rad/IO/Logging.h>

#include <gtest/gtest.h>

int main(int argc, char** argv)
{
    rad::Application app(argc, argv);
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
