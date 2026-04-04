#include <rad/Common/String.h>

#include <list>
#include <vector>

#include <gtest/gtest.h>

void TestStrSplit()
{
    {
        std::string str = "Aa;Bb;Cc;Dd;Ee;Ff;Gg";
        auto tokens = rad::StrSplit(str, ";", true);
        EXPECT_EQ(tokens.size(), 7);
        for (size_t i = 0; i < tokens.size(); ++i)
        {
            EXPECT_EQ(tokens[i].size(), 2);
            EXPECT_EQ(tokens[i][0], static_cast<char>('A' + i));
            EXPECT_EQ(tokens[i][1], static_cast<char>('a' + i));
        }
    }
    {
        std::string str = "Aa;Bb;Cc;Dd;Ee;Ff;Gg";
        auto tokens = rad::StrSplitViews(str, ";", true);
        EXPECT_EQ(tokens.size(), 7);
        for (size_t i = 0; i < tokens.size(); ++i)
        {
            EXPECT_EQ(tokens[i].size(), 2);
            EXPECT_EQ(tokens[i][0], static_cast<char>('A' + i));
            EXPECT_EQ(tokens[i][1], static_cast<char>('a' + i));
        }
    }
}

void TestRangeToString()
{
    {
        std::vector<int> vec = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        EXPECT_EQ(rad::RangeToString(vec), "0, 1, 2, 3, 4, 5, 6, 7, 8, 9");
    }
    {
        std::vector<float> vec = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        EXPECT_EQ(rad::RangeToString(vec, "{:.2f}"), "0.00, 1.00, 2.00, 3.00, 4.00, 5.00, 6.00, 7.00, 8.00, 9.00");
    }
}

TEST(Common, String)
{
    TestStrSplit();
    TestRangeToString();
}
