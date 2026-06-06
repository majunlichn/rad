#include <rad/Common/Span.h>

#include <array>
#include <numeric>
#include <vector>

#include <gtest/gtest.h>

[[nodiscard]] int Sum(rad::Span<const int> span)
{
    return std::accumulate(span.begin(), span.end(), 0);
}

TEST(Common, Span)
{
    EXPECT_EQ(Sum(1), 1);
    EXPECT_EQ(Sum({1, 2, 3, 4, 5}), 15);

    int arr[] = {1, 2, 3, 4, 5};
    EXPECT_EQ(Sum(arr), 15);

    std::array<int, 5> array = {1, 2, 3, 4, 5};
    EXPECT_EQ(Sum(array), 15);
    EXPECT_EQ(Sum(std::array<int, 5>{1, 2, 3, 4, 5}), 15);

    std::vector<int> vec = {1, 2, 3, 4, 5};
    EXPECT_EQ(Sum(vec), 15);
    EXPECT_EQ(Sum(std::vector<int>{1, 2, 3, 4, 5}), 15);

    EXPECT_EQ(Sum(rad::MakeSpan(arr)), 15);
    EXPECT_EQ(Sum(rad::MakeSpan(vec)), 15);
}
