#include <rad/Common/Math.h>

#include <gtest/gtest.h>

using namespace rad;

namespace
{
constexpr double kEps = 1e-12;
} // namespace

TEST(Math, SolveQuadratic)
{
    double t0{};
    double t1{};

    {
        // Degenerate linear: bt + c = 0 with b != 0 -> t = -c/b
        EXPECT_TRUE(SolveQuadratic(0.0, 2.0, -6.0, t0, t1));
        EXPECT_NEAR(t0, 3.0, kEps);
        EXPECT_NEAR(t1, 3.0, kEps);
    }

    {
        // a == 0, b == 0: constant equation
        EXPECT_TRUE(SolveQuadratic(0.0, 0.0, 0.0, t0, t1));
        EXPECT_DOUBLE_EQ(t0, 0.0);
        EXPECT_DOUBLE_EQ(t1, 0.0);

        EXPECT_FALSE(SolveQuadratic(0.0, 0.0, 1.0, t0, t1));
    }

    {
        // No real roots
        EXPECT_FALSE(SolveQuadratic(1.0, 0.0, 1.0, t0, t1));
        EXPECT_FALSE(SolveQuadratic(1.0, 0.0, 2.0, t0, t1));
        EXPECT_FALSE(SolveQuadratic(2.0, 2.0, 3.0, t0, t1));
    }

    {
        // Repeated roots
        EXPECT_TRUE(SolveQuadratic(1.0, 2.0, 1.0, t0, t1));
        EXPECT_NEAR(t0, -1.0, kEps);
        EXPECT_NEAR(t1, -1.0, kEps);

        EXPECT_TRUE(SolveQuadratic(1.0, -6.0, 9.0, t0, t1));
        EXPECT_NEAR(t0, 3.0, kEps);
        EXPECT_NEAR(t1, 3.0, kEps);

        EXPECT_TRUE(SolveQuadratic(4.0, 0.0, 0.0, t0, t1));
        EXPECT_NEAR(t0, 0.0, kEps);
        EXPECT_NEAR(t1, 0.0, kEps);
    }

    {
        // Two distinct roots (and ordering t0 <= t1)
        EXPECT_TRUE(SolveQuadratic(1.0, -5.0, 6.0, t0, t1));
        EXPECT_NEAR(t0, 2.0, kEps);
        EXPECT_NEAR(t1, 3.0, kEps);

        EXPECT_TRUE(SolveQuadratic(1.0, 5.0, 6.0, t0, t1));
        EXPECT_NEAR(t0, -3.0, kEps);
        EXPECT_NEAR(t1, -2.0, kEps);

        EXPECT_TRUE(SolveQuadratic(1.0, 0.0, -1.0, t0, t1));
        EXPECT_NEAR(t0, -1.0, kEps);
        EXPECT_NEAR(t1, 1.0, kEps);

        EXPECT_TRUE(SolveQuadratic(2.0, 5.0, 2.0, t0, t1));
        EXPECT_NEAR(t0, -2.0, kEps);
        EXPECT_NEAR(t1, -0.5, kEps);

        EXPECT_TRUE(SolveQuadratic(-1.0, 1.0, 2.0, t0, t1));
        EXPECT_NEAR(t0, -1.0, kEps);
        EXPECT_NEAR(t1, 2.0, kEps);
    }

    {
        // float template instantiation
        float tf0{};
        float tf1{};
        EXPECT_TRUE(SolveQuadratic(1.f, -5.f, 6.f, tf0, tf1));
        EXPECT_NEAR(tf0, 2.f, 1e-6f);
        EXPECT_NEAR(tf1, 3.f, 1e-6f);
    }
}
