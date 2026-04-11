#include <rad/Common/Math.h>

#include <iostream>

#include <gtest/gtest.h>

// Helper to verify that t is an actual root of at^2 + bt + c = 0.
template <typename Float>
void ExpectQuadraticRoot(Float a, Float b, Float c, Float t, Float tolerance = Float(1e-6))
{
    Float result = a * t * t + b * t + c;
    EXPECT_NEAR(result, Float(0), tolerance)
        << "  root t=" << t << " is not valid for " << a << "*t^2 + " << b << "*t + " << c;
}

void TestQuadraticEquation()
{
    { // TwoDistinctRoots: t^2 - 5t + 6 = 0 => t=2, t=3
        double t0 = 0, t1 = 0;
        EXPECT_TRUE(rad::SolveQuadratic(1.0, -5.0, 6.0, t0, t1));
        EXPECT_LE(t0, t1);
        EXPECT_NEAR(t0, 2.0, 1e-9);
        EXPECT_NEAR(t1, 3.0, 1e-9);
        ExpectQuadraticRoot(1.0, -5.0, 6.0, t0);
        ExpectQuadraticRoot(1.0, -5.0, 6.0, t1);
    }
    { // TwoDistinctRoots (float): t^2 - 5t + 6 = 0 => t=2, t=3
        float t0 = 0, t1 = 0;
        EXPECT_TRUE(rad::SolveQuadratic(1.0f, -5.0f, 6.0f, t0, t1));
        EXPECT_LE(t0, t1);
        EXPECT_NEAR(t0, 2.0f, 1e-5f);
        EXPECT_NEAR(t1, 3.0f, 1e-5f);
        ExpectQuadraticRoot(1.0f, -5.0f, 6.0f, t0);
        ExpectQuadraticRoot(1.0f, -5.0f, 6.0f, t1);
    }
    { // RepeatedRoot: t^2 - 4t + 4 = 0 => t=2
        double t0 = 0, t1 = 0;
        EXPECT_TRUE(rad::SolveQuadratic(1.0, -4.0, 4.0, t0, t1));
        EXPECT_NEAR(t0, 2.0, 1e-9);
        EXPECT_NEAR(t1, 2.0, 1e-9);
    }
    { // NoRealRoots: t^2 + t + 1 = 0
        double t0 = 0, t1 = 0;
        EXPECT_FALSE(rad::SolveQuadratic(1.0, 1.0, 1.0, t0, t1));
    }
    { // Linear (a=0): 3t + 6 = 0 => t=-2
        double t0 = 0, t1 = 0;
        EXPECT_TRUE(rad::SolveQuadratic(0.0, 3.0, 6.0, t0, t1));
        EXPECT_NEAR(t0, -2.0, 1e-9);
        EXPECT_NEAR(t1, -2.0, 1e-9);
    }
    { // AllZero: a=0, b=0, c=0 => trivially 0=0
        double t0 = 0, t1 = 0;
        EXPECT_TRUE(rad::SolveQuadratic(0.0, 0.0, 0.0, t0, t1));
        EXPECT_EQ(t0, 0.0);
        EXPECT_EQ(t1, 0.0);
    }
    { // NoSolution: a=0, b=0, c!=0 => 5=0 impossible
        double t0 = 0, t1 = 0;
        EXPECT_FALSE(rad::SolveQuadratic(0.0, 0.0, 5.0, t0, t1));
    }
    { // SymmetricRoots: t^2 - 9 = 0 => t=-3, t=3
        double t0 = 0, t1 = 0;
        EXPECT_TRUE(rad::SolveQuadratic(1.0, 0.0, -9.0, t0, t1));
        EXPECT_LE(t0, t1);
        EXPECT_NEAR(t0, -3.0, 1e-9);
        EXPECT_NEAR(t1, 3.0, 1e-9);
    }
    { // NegativeA: -t^2 + 3t - 2 = 0 => t=1, t=2
        double t0 = 0, t1 = 0;
        EXPECT_TRUE(rad::SolveQuadratic(-1.0, 3.0, -2.0, t0, t1));
        EXPECT_LE(t0, t1);
        EXPECT_NEAR(t0, 1.0, 1e-9);
        EXPECT_NEAR(t1, 2.0, 1e-9);
    }
    { // OneRootZero: t^2 - 5t = 0 => t=0, t=5
        double t0 = 0, t1 = 0;
        EXPECT_TRUE(rad::SolveQuadratic(1.0, -5.0, 0.0, t0, t1));
        EXPECT_LE(t0, t1);
        EXPECT_NEAR(t0, 0.0, 1e-9);
        EXPECT_NEAR(t1, 5.0, 1e-9);
    }
    { // BothRootsNegative: t^2 + 5t + 6 = 0 => t=-3, t=-2
        double t0 = 0, t1 = 0;
        EXPECT_TRUE(rad::SolveQuadratic(1.0, 5.0, 6.0, t0, t1));
        EXPECT_LE(t0, t1);
        EXPECT_NEAR(t0, -3.0, 1e-9);
        EXPECT_NEAR(t1, -2.0, 1e-9);
    }
    { // LargeCoefficients: 1e6*t^2 - 3e6*t + 2e6 = 0 => t=1, t=2
        double t0 = 0, t1 = 0;
        EXPECT_TRUE(rad::SolveQuadratic(1e6, -3e6, 2e6, t0, t1));
        EXPECT_LE(t0, t1);
        EXPECT_NEAR(t0, 1.0, 1e-6);
        EXPECT_NEAR(t1, 2.0, 1e-6);
        ExpectQuadraticRoot(1e6, -3e6, 2e6, t0);
        ExpectQuadraticRoot(1e6, -3e6, 2e6, t1);
    }
    { // SmallCoefficients: 1e-8*t^2 - 3e-8*t + 2e-8 = 0 => t=1, t=2
        double t0 = 0, t1 = 0;
        EXPECT_TRUE(rad::SolveQuadratic(1e-8, -3e-8, 2e-8, t0, t1));
        EXPECT_LE(t0, t1);
        EXPECT_NEAR(t0, 1.0, 1e-6);
        EXPECT_NEAR(t1, 2.0, 1e-6);
    }
    { // OrderingGuarantee: 2t^2 + 7t + 3 = 0 => t=-3, t=-0.5
        double t0 = 0, t1 = 0;
        EXPECT_TRUE(rad::SolveQuadratic(2.0, 7.0, 3.0, t0, t1));
        EXPECT_LE(t0, t1);
        EXPECT_NEAR(t0, -3.0, 1e-9);
        EXPECT_NEAR(t1, -0.5, 1e-9);
    }
}

TEST(Common, Math)
{
    TestQuadraticEquation();
}
