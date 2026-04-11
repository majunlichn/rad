#include <rad/Common/String.h>

#include <deque>
#include <list>
#include <set>
#include <vector>

#include <gtest/gtest.h>

void TestStrPrintfV()
{
    EXPECT_EQ(rad::StrPrintf("%s", "Hello"), "Hello");
    // Fits probe buffer exactly (15 chars + NUL).
    EXPECT_EQ(rad::StrPrintf("%s", "123456789012345"), "123456789012345");
    // Needs one byte more than probe buffer (16 data chars).
    EXPECT_EQ(rad::StrPrintf("%s", "1234567890123456"), "1234567890123456");
}

void TestStrSplit()
{
    {
        std::string str = "Aa;Bb;Cc;Dd;Ee;Ff;Gg;";
        auto tokens = rad::StrSplit(str, ";", true);
        ASSERT_EQ(tokens.size(), 7);
        EXPECT_EQ(tokens[0], "Aa");
        EXPECT_EQ(tokens[1], "Bb");
        EXPECT_EQ(tokens[2], "Cc");
        EXPECT_EQ(tokens[3], "Dd");
        EXPECT_EQ(tokens[4], "Ee");
        EXPECT_EQ(tokens[5], "Ff");
        EXPECT_EQ(tokens[6], "Gg");
    }
    {
        std::string str = "Aa;Bb;Cc;Dd;Ee;Ff;Gg;";
        auto tokens = rad::StrSplitViews(str, ";", true);
        ASSERT_EQ(tokens.size(), 7);
        EXPECT_EQ(tokens[0], "Aa");
        EXPECT_EQ(tokens[1], "Bb");
        EXPECT_EQ(tokens[2], "Cc");
        EXPECT_EQ(tokens[3], "Dd");
        EXPECT_EQ(tokens[4], "Ee");
        EXPECT_EQ(tokens[5], "Ff");
        EXPECT_EQ(tokens[6], "Gg");
    }
}

void TestRangeToString()
{
    {
        int arr[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        EXPECT_EQ(rad::RangeToString(arr), "0, 1, 2, 3, 4, 5, 6, 7, 8, 9");
    }
    {
        std::vector<int> vec = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        EXPECT_EQ(rad::RangeToString(vec), "0, 1, 2, 3, 4, 5, 6, 7, 8, 9");
    }
    {
        std::list<float> list = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        EXPECT_EQ(rad::RangeToString(list, "{:.2f}"),
                  "0.00, 1.00, 2.00, 3.00, 4.00, 5.00, 6.00, 7.00, 8.00, 9.00");
    }
    {
        std::deque<int> deque = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        EXPECT_EQ(rad::RangeToString(deque), "0, 1, 2, 3, 4, 5, 6, 7, 8, 9");
    }
}

void TestStringLessCaseInsensitive()
{
    const rad::StringLessCaseInsensitive less{};

    // Lexicographic by character, not by length alone (case-insensitive).
    EXPECT_TRUE(less("aa", "b"));
    EXPECT_FALSE(less("b", "aa"));

    // Shorter prefix orders before longer when the prefix matches.
    EXPECT_TRUE(less("a", "aa"));
    EXPECT_FALSE(less("aa", "a"));

    // Equality under comparison: neither is less than the other.
    EXPECT_FALSE(less("Aa", "aa"));
    EXPECT_FALSE(less("aa", "Aa"));

    {
        // Case-only variants are equivalent keys in std::set (only one is kept).
        std::set<std::string, rad::StringLessCaseInsensitive> stringSet{"Aa", "aa"};
        EXPECT_EQ(stringSet.size(), 1u);
    }
    {
        // Iteration order is case-insensitive lexicographic.
        std::set<std::string, rad::StringLessCaseInsensitive> stringSet{"b", "Aa", "a"};
        std::vector<std::string> actual(stringSet.begin(), stringSet.end());
        EXPECT_EQ(actual, (std::vector<std::string>{"a", "Aa", "b"}));
    }
}

TEST(Common, String)
{
    TestStrPrintfV();
    TestStrSplit();
    TestRangeToString();

    TestStringLessCaseInsensitive();
}
