#include <rad/Common/Float.h>
#include <rad/Common/Float16.h>
#include <rad/Common/BFloat16.h>
#include <rad/Common/Float8.h>

#include <format>
#include <iostream>
#include <random>

#include <gtest/gtest.h>

template <std::integral T>
void TestQuantization()
{
    constexpr int TestCount = 10000;

    std::default_random_engine rng;
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    float maxError = 0.0f;
    double sumError = 0.0;
    for (int i = 0; i < TestCount; ++i)
    {
        float original = dist(rng);
        T quantized = rad::QuantizeUnorm<T>(original, 0.0f, 1.0f);
        float dequantized = rad::DequantizeUnorm<T>(quantized);

        float error = std::abs(original - dequantized);
        maxError = std::max(maxError, error);
        sumError += error;
        EXPECT_LE(error, 0.5f / float(std::numeric_limits<T>::max()));
    }

    float avgError = float(sumError / TestCount);
    std::cout << std::format("QuantizeUnorm{}: MaxError={:e}; AvgError={:e}; ", sizeof(T) * 8,
                             maxError, avgError)
              << std::endl;
}

void TestFloat16()
{
    rad::Float16 a = static_cast<rad::Float16>(3.4);
    rad::Float16 b(5);
    rad::Float16 c = a * b;
    c += 3;
    EXPECT_NEAR(c, 3.4 * 5 + 3, 0.001);

    // Test fp16_ieee_to_fp32_bits edge cases and standard values
    EXPECT_EQ(rad::fp16_ieee_to_fp32_bits(0x0000), 0x00000000); // +0.0
    EXPECT_EQ(rad::fp16_ieee_to_fp32_bits(0x8000), 0x80000000); // -0.0
    EXPECT_EQ(rad::fp16_ieee_to_fp32_bits(0x3C00), 0x3F800000); // +1.0
    EXPECT_EQ(rad::fp16_ieee_to_fp32_bits(0xBC00), 0xBF800000); // -1.0
}

void TestBFloat16()
{
    rad::BFloat16 a = static_cast<rad::BFloat16>(3.4);
    rad::BFloat16 b(5);
    rad::BFloat16 c = a * b;
    c += 3;
    EXPECT_NEAR(c, 3.4 * 5 + 3, 0.01);
}

void TestFloat8E4M3()
{
    rad::Float8E4M3 a(3.4);
    rad::Float8E4M3 b(5);
    rad::Float8E4M3 c = a * b;
    c += 3;
    EXPECT_NEAR(c, 3.4 * 5 + 3, 0.01);
}

void TestFloat8E5M2()
{
    rad::Float8E5M2 a(3.4);
    rad::Float8E5M2 b(5);
    rad::Float8E5M2 c = a * b;
    c += 3;
    EXPECT_NEAR(c, 3.4 * 5 + 3, 0.1);
}

TEST(Common, Float)
{
    TestQuantization<uint8_t>();
    TestQuantization<uint16_t>();
    TestFloat16();
    TestBFloat16();
    TestFloat8E4M3();
    TestFloat8E5M2();
}
