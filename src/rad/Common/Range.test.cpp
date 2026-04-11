#include <rad/Common/Range.h>

#include <random>

#include <gtest/gtest.h>

template <std::ranges::random_access_range Range, typename Compare = std::ranges::less>
bool IsSorted(Range&& vec, Compare comp = {})
{
    for (size_t i = 1; i < vec.size(); ++i)
    {
        if (comp(vec[i], vec[i - 1]))
        {
            return false;
        }
    }
    return true;
}

template <typename ExecutionPolicy = std::execution::sequenced_policy,
          typename Compare = std::ranges::less>
void TestSortIndices(ExecutionPolicy policy = std::execution::seq, Compare comp = {})
{
    size_t count = 1024;
    std::default_random_engine rng;
    std::uniform_int_distribution<int> dist(0, count);
    std::vector<int> v;
    for (size_t i = 0; i < count; ++i)
    {
        v.push_back(dist(rng));
    }
    auto indices = rad::SortIndices(v, policy, comp);
    std::vector<size_t> sorted;
    sorted.reserve(indices.size());
    for (size_t i : indices)
    {
        sorted.push_back(v[i]);
    }
    EXPECT_TRUE(IsSorted(sorted, comp));
}

void TestSlice()
{
    std::vector<int> list = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::vector<int> expected;

    auto sliced = rad::Slice(list, 2, 6);
    expected = {2, 3, 4, 5};
    EXPECT_EQ(sliced, expected);
    sliced = rad::Slice(list, 2, 6, 1);
    expected = {2, 3, 4, 5};
    EXPECT_EQ(sliced, expected);
    sliced = rad::Slice(list, 2, 6, 2);
    expected = {2, 4};
    EXPECT_EQ(sliced, expected);
    sliced = rad::Slice(list, 6, 2, -1);
    expected = {6, 5, 4, 3};
    EXPECT_EQ(sliced, expected);
    sliced = rad::Slice(list, 6, 2, -2);
    expected = {6, 4};
    EXPECT_EQ(sliced, expected);

    sliced = rad::Slice(list, -8, -4);
    expected = {2, 3, 4, 5};
    EXPECT_EQ(sliced, expected);
    sliced = rad::Slice(list, -8, -4, 1);
    expected = {2, 3, 4, 5};
    EXPECT_EQ(sliced, expected);
    sliced = rad::Slice(list, -8, -4, 2);
    expected = {2, 4};
    EXPECT_EQ(sliced, expected);
    sliced = rad::Slice(list, -4, -8, -1);
    expected = {6, 5, 4, 3};
    EXPECT_EQ(sliced, expected);
    sliced = rad::Slice(list, -4, -8, -2);
    expected = {6, 4};
    EXPECT_EQ(sliced, expected);

    sliced = rad::Slice(list, 0, 10);
    expected = list;
    EXPECT_EQ(sliced, expected);
    sliced = rad::Slice(list, 0, 10, 1);
    expected = list;
    EXPECT_EQ(sliced, expected);
    sliced = rad::Slice(list, -20, -10);
    expected = {};
    EXPECT_EQ(sliced, expected);
    sliced = rad::Slice(list, -15, 5);
    expected = {0, 1, 2, 3, 4};
    EXPECT_EQ(sliced, expected);
    sliced = rad::Slice(list, 5, 15);
    expected = {5, 6, 7, 8, 9};
    EXPECT_EQ(sliced, expected);
    sliced = rad::Slice(list, 10, 20);
    expected = {};
    EXPECT_EQ(sliced, expected);

    sliced = rad::Slice(list, 5, -100, -1);
    expected = {5, 4, 3, 2, 1, 0};
    EXPECT_EQ(sliced, expected);
}

TEST(Common, Range)
{
    TestSortIndices(std::execution::seq, std::ranges::less{});
    TestSortIndices(std::execution::seq, std::ranges::greater{});
    TestSortIndices(std::execution::par, std::ranges::less{});
    TestSortIndices(std::execution::par, std::ranges::greater{});

    TestSlice();
}
