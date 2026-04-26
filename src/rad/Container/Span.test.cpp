#include <rad/Container/Span.h>

#include <algorithm>
#include <array>
#include <initializer_list>
#include <numeric>
#include <ranges>
#include <vector>

#include <gtest/gtest.h>

[[nodiscard]] int Sum(rad::Span<const int> span)
{
    return std::accumulate(span.begin(), span.end(), 0);
}

TEST(Container, Span)
{
    int result = 0;
    {
        result = Sum(rad::MakeSpan(1));
        EXPECT_EQ(result, 1);
    }
    {
        std::initializer_list<int> list = {1, 2, 3, 4, 5};
        result = Sum(list);
        EXPECT_EQ(result, 15);
        result = Sum(rad::MakeSpan(list));
        EXPECT_EQ(result, 15);
        result = Sum(rad::MakeSpan({1, 2, 3, 4, 5}));
        EXPECT_EQ(result, 15);
    }
    {
        int list[] = {1, 2, 3, 4, 5};
        result = Sum(list);
        EXPECT_EQ(result, 15);
        result = Sum(rad::MakeSpan(list));
        EXPECT_EQ(result, 15);
    }
    {
        std::array<int, 5> list = {1, 2, 3, 4, 5};
        result = Sum(list);
        EXPECT_EQ(result, 15);
        result = Sum(rad::MakeSpan(list));
        EXPECT_EQ(result, 15);
        result = Sum(rad::MakeSpan(std::array<int, 5>{1, 2, 3, 4, 5}));
        EXPECT_EQ(result, 15);
    }
    {
        std::vector<int> list = {1, 2, 3, 4, 5};
        result = Sum(list);
        EXPECT_EQ(result, 15);
        result = Sum(rad::MakeSpan(list));
        EXPECT_EQ(result, 15);
        result = Sum(rad::MakeSpan(std::vector<int>{1, 2, 3, 4, 5}));
        EXPECT_EQ(result, 15);
    }
}
