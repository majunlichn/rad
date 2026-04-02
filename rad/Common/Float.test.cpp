#include <rad/Common/Float.h>

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
    std::cout << std::format("QuantizeUnorm{}: MaxError={:e}; AvgError={:e}; ", sizeof(T) * 8, maxError, avgError)
              << std::endl;
}

TEST(Common, Float)
{
    TestQuantization<uint8_t>();
    TestQuantization<uint16_t>();
}
