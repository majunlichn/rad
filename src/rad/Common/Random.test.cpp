#include <rad/Common/Random.h>

#include <cmath>

#include <format>
#include <map>
#include <string>

#include <gtest/gtest.h>

void CheckNormalDistribution(double mean, double stddev, int N, const std::map<int, int>& hist)
{
    auto Phi = [](double z) -> double { return 0.5 * (1.0 + std::erf(z / std::sqrt(2.0))); };

    for (auto [k, count] : hist)
    {
        double z_lower = (double(k) - 0.5 - mean) / stddev;
        double z_upper = (double(k) + 0.5 - mean) / stddev;
        double p = Phi(z_upper) - Phi(z_lower);

        int expected_count = static_cast<int>(std::round(N * p));
        if (expected_count < 5)
        {
            continue; // skip low-statistics buckets
        }

        double sigma = std::sqrt(N * p * (1.0 - p));
        int tolerance = static_cast<int>(5.0 * sigma) + 1;

        std::cout << std::format(
            "Bucket {:4}: Count={:6}, Expected={:6}, Diff={:6} (Tolerance={:6})\n", k, count,
            expected_count, count - expected_count, tolerance);
        EXPECT_NEAR(count, expected_count, tolerance);
    }
}

// https://www.pcg-random.org/using-pcg-cpp.html
void TestNormalDistribution(float mean, float stddev, int N)
{
    std::cout << std::format("TestNormalDistribution: mean={}, stddev={}, N={}", mean, stddev, N)
              << '\n';

    // Seed with a real random value, if available
    pcg_extras::seed_seq_from<std::random_device> seed_source;

    // Make a random number engine
    pcg32 rng(seed_source);
    std::normal_distribution<float> normal_dist(mean, stddev);

    // Produce histogram
    std::map<int, int> hist;
    // Make a copy of the RNG state to use later
    pcg32 rng_checkpoint = rng;
    for (int n = 0; n < N; ++n)
    {
        ++hist[static_cast<int>(std::nearbyintf(normal_dist(rng)))];
    }
    // Produce information about RNG usage
    std::cout << "Required " << (rng - rng_checkpoint) << " random numbers.\n";

    // Verify the normal distribution:
    CheckNormalDistribution(mean, stddev, N, hist);
}

TEST(Common, Random)
{
    TestNormalDistribution(0.0f, 0.5f, 10000);
    TestNormalDistribution(0.0f, 1.0f, 10000);
    TestNormalDistribution(0.0f, 2.0f, 10000);
}
