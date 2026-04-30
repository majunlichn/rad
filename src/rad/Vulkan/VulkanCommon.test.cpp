#include <rad/Vulkan/VulkanCommon.h>

#include <gtest/gtest.h>

TEST(Vulkan, Common)
{
    rad::VulkanVersion version(1, 4, 341);
    EXPECT_EQ(version.GetMajor(), 1);
    EXPECT_EQ(version.GetMinor(), 4);
    EXPECT_EQ(version.GetPatch(), 341);
}
