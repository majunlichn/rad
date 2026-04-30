#include <rad/Common/Float.h>

#include <gtest/gtest.h>

#include <limits>

void TestFp32BitCasts()
{
    // Basic round-trips (including signed zero).
    EXPECT_EQ(rad::fp32_to_bits(rad::fp32_from_bits(0x00000000u)), 0x00000000u);
    EXPECT_EQ(rad::fp32_to_bits(rad::fp32_from_bits(0x80000000u)), 0x80000000u);

    // Infinities.
    EXPECT_EQ(rad::fp32_to_bits(rad::fp32_from_bits(0x7F800000u)), 0x7F800000u);
    EXPECT_EQ(rad::fp32_to_bits(rad::fp32_from_bits(0xFF800000u)), 0xFF800000u);

    // NaNs (payload should survive bit_cast round-trip).
    EXPECT_EQ(rad::fp32_to_bits(rad::fp32_from_bits(0x7FC00001u)), 0x7FC00001u);
    EXPECT_EQ(rad::fp32_to_bits(rad::fp32_from_bits(0xFFC12345u)), 0xFFC12345u);
}

void TestAlmostEqual()
{
    constexpr float eps = rad::Epsilon<float>;

    EXPECT_TRUE(rad::AlmostEqual(1.0f, 1.0f));
    EXPECT_TRUE(rad::AlmostEqual(0.0f, 0.0f));

    // Near zero: scale is clamped to 1.
    EXPECT_TRUE(rad::AlmostEqual(0.0f, eps * 0.5f, eps));
    EXPECT_FALSE(rad::AlmostEqual(0.0f, eps * 2.0f, eps));

    // Relative comparisons at larger magnitudes.
    EXPECT_TRUE(rad::AlmostEqual(1000.0f, 1000.0f + 1000.0f * eps * 0.5f, eps));
    EXPECT_FALSE(rad::AlmostEqual(1000.0f, 1000.0f + 1000.0f * eps * 4.0f, eps));

    // NaN never compares equal.
    constexpr float qnan = std::numeric_limits<float>::quiet_NaN();
    EXPECT_FALSE(rad::AlmostEqual(qnan, qnan));
    EXPECT_FALSE(rad::AlmostEqual(0.0f, qnan));

    // Infinities compare equal only by exact equality.
    constexpr float inf = std::numeric_limits<float>::infinity();
    EXPECT_TRUE(rad::AlmostEqual(inf, inf));
    EXPECT_FALSE(rad::AlmostEqual(inf, -inf));
}

void TestAlmostZero()
{
    constexpr float eps = rad::Epsilon<float>;

    EXPECT_TRUE(rad::AlmostZero(0.0f, eps));
    EXPECT_TRUE(rad::AlmostZero(eps * 0.5f, eps));
    EXPECT_FALSE(rad::AlmostZero(eps * 2.0f, eps));
}

void TestNormalize()
{
    EXPECT_FLOAT_EQ(rad::Normalize(0.0f, -1.0f, 1.0f), 0.5f);
    EXPECT_FLOAT_EQ(rad::Normalize(-2.0f, -1.0f, 1.0f), 0.0f);
    EXPECT_FLOAT_EQ(rad::Normalize(2.0f, -1.0f, 1.0f), 1.0f);
    EXPECT_FLOAT_EQ(rad::Normalize(1.0f, -1.0f, 1.0f), 1.0f);
    EXPECT_FLOAT_EQ(rad::Normalize(-1.0f, -1.0f, 1.0f), 0.0f);
}

void TestQuantizeDequantizeUnorm()
{
    // Quantize endpoints.
    EXPECT_EQ(rad::QuantizeUnorm8(0.0f, 0.0f, 1.0f), 0u);
    EXPECT_EQ(rad::QuantizeUnorm8(1.0f, 0.0f, 1.0f), 255u);
    EXPECT_EQ(rad::QuantizeUnorm16(0.0f, 0.0f, 1.0f), 0u);
    EXPECT_EQ(rad::QuantizeUnorm16(1.0f, 0.0f, 1.0f), 65535u);

    // Midpoint with nearbyintf (ties-to-even): 0.5 * 255 = 127.5 -> 128.
    EXPECT_EQ(rad::QuantizeUnorm8(0.5f, 0.0f, 1.0f), 128u);
    EXPECT_EQ(rad::QuantizeUnorm16(0.5f, 0.0f, 1.0f), 32768u);

    // Dequantize endpoints.
    EXPECT_FLOAT_EQ(rad::DequantizeUnorm8(0u), 0.0f);
    EXPECT_FLOAT_EQ(rad::DequantizeUnorm8(255u), 1.0f);
    EXPECT_FLOAT_EQ(rad::DequantizeUnorm16(0u), 0.0f);
    EXPECT_FLOAT_EQ(rad::DequantizeUnorm16(65535u), 1.0f);
}

TEST(Common, Float)
{
    TestFp32BitCasts();
    TestAlmostEqual();
    TestAlmostZero();
    TestNormalize();
    TestQuantizeDequantizeUnorm();
}

