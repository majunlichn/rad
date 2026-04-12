#include <rad/Vulkan/VulkanCommon.h>

#include <gtest/gtest.h>

TEST(Vulkan, VulkanCommon)
{
    rad::VulkanVersion version(1, 2, 3, 4);
    EXPECT_EQ(version.GetVariant(), 1);
    EXPECT_EQ(version.GetMajor(), 2);
    EXPECT_EQ(version.GetMinor(), 3);
    EXPECT_EQ(version.GetPatch(), 4);
}
