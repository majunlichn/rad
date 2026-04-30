#include <rad/Common/Float.h>

#include <gtest/gtest.h>

#include <limits>

using namespace rad;

TEST(Float, Fp32BitCasts)
{
    // Basic round-trips (including signed zero).
    EXPECT_EQ(fp32_to_bits(fp32_from_bits(0x00000000u)), 0x00000000u);
    EXPECT_EQ(fp32_to_bits(fp32_from_bits(0x80000000u)), 0x80000000u);

    // Infinities.
    EXPECT_EQ(fp32_to_bits(fp32_from_bits(0x7F800000u)), 0x7F800000u);
    EXPECT_EQ(fp32_to_bits(fp32_from_bits(0xFF800000u)), 0xFF800000u);

    // NaNs (payload should survive bit_cast round-trip).
    EXPECT_EQ(fp32_to_bits(fp32_from_bits(0x7FC00001u)), 0x7FC00001u);
    EXPECT_EQ(fp32_to_bits(fp32_from_bits(0xFFC12345u)), 0xFFC12345u);
}

TEST(Float, AlmostEqual)
{
    constexpr float eps = Epsilon<float>;

    EXPECT_TRUE(AlmostEqual(1.0f, 1.0f));
    EXPECT_TRUE(AlmostEqual(0.0f, 0.0f));

    // Near zero: scale is clamped to 1.
    EXPECT_TRUE(AlmostEqual(0.0f, eps * 0.5f, eps));
    EXPECT_FALSE(AlmostEqual(0.0f, eps * 2.0f, eps));

    // Relative comparisons at larger magnitudes.
    EXPECT_TRUE(AlmostEqual(1000.0f, 1000.0f + 1000.0f * eps * 0.5f, eps));
    EXPECT_FALSE(AlmostEqual(1000.0f, 1000.0f + 1000.0f * eps * 4.0f, eps));

    // NaN never compares equal.
    constexpr float qnan = std::numeric_limits<float>::quiet_NaN();
    EXPECT_FALSE(AlmostEqual(qnan, qnan));
    EXPECT_FALSE(AlmostEqual(0.0f, qnan));

    // Infinities compare equal only by exact equality.
    constexpr float inf = std::numeric_limits<float>::infinity();
    EXPECT_TRUE(AlmostEqual(inf, inf));
    EXPECT_FALSE(AlmostEqual(inf, -inf));
}

TEST(Float, AlmostZero)
{
    constexpr float eps = Epsilon<float>;

    EXPECT_TRUE(AlmostZero(0.0f, eps));
    EXPECT_TRUE(AlmostZero(eps * 0.5f, eps));
    EXPECT_FALSE(AlmostZero(eps * 2.0f, eps));
}

TEST(Float, Normalize)
{
    EXPECT_FLOAT_EQ(Normalize(0.0f, -1.0f, 1.0f), 0.5f);
    EXPECT_FLOAT_EQ(Normalize(-2.0f, -1.0f, 1.0f), 0.0f);
    EXPECT_FLOAT_EQ(Normalize(2.0f, -1.0f, 1.0f), 1.0f);
    EXPECT_FLOAT_EQ(Normalize(1.0f, -1.0f, 1.0f), 1.0f);
    EXPECT_FLOAT_EQ(Normalize(-1.0f, -1.0f, 1.0f), 0.0f);
}

TEST(Float, QuantizeDequantizeUnorm)
{
    // Quantize endpoints.
    EXPECT_EQ(QuantizeUnorm8(0.0f, 0.0f, 1.0f), 0u);
    EXPECT_EQ(QuantizeUnorm8(1.0f, 0.0f, 1.0f), 255u);
    EXPECT_EQ(QuantizeUnorm16(0.0f, 0.0f, 1.0f), 0u);
    EXPECT_EQ(QuantizeUnorm16(1.0f, 0.0f, 1.0f), 65535u);

    // Midpoint with nearbyintf (ties-to-even): 0.5 * 255 = 127.5 -> 128.
    EXPECT_EQ(QuantizeUnorm8(0.5f, 0.0f, 1.0f), 128u);
    EXPECT_EQ(QuantizeUnorm16(0.5f, 0.0f, 1.0f), 32768u);

    // Dequantize endpoints.
    EXPECT_FLOAT_EQ(DequantizeUnorm8(0u), 0.0f);
    EXPECT_FLOAT_EQ(DequantizeUnorm8(255u), 1.0f);
    EXPECT_FLOAT_EQ(DequantizeUnorm16(0u), 0.0f);
    EXPECT_FLOAT_EQ(DequantizeUnorm16(65535u), 1.0f);
}
