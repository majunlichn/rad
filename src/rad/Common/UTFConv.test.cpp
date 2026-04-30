#include <rad/Common/UTFConv.h>

#include <gtest/gtest.h>

#include <cstring>
#include <string>
#include <string_view>

using namespace rad;

// Helper to compare std::u8string without using EXPECT_EQ directly.
#define EXPECT_U8STRING_EQ(val1, val2) EXPECT_EQ(UTFConv::ToUTF8(val1), UTFConv::ToUTF8(val2))

namespace
{
struct UTFConvTestStrings
{
    std::string asciiNarrow = "Hello, World!";
    std::wstring asciiWide = L"Hello, World!";
    std::u8string asciiUtf8 = u8"Hello, World!";
    std::u16string asciiUtf16 = u"Hello, World!";
    std::u32string asciiUtf32 = U"Hello, World!";

    std::string latinNarrow = "Café résumé naïve";
    std::wstring latinWide = L"Café résumé naïve";
    std::u8string latinUtf8 = u8"Café résumé naïve";
    std::u16string latinUtf16 = u"Café résumé naïve";
    std::u32string latinUtf32 = U"Café résumé naïve";

    std::string japaneseNarrow = "こんにちは世界";
    std::wstring japaneseWide = L"こんにちは世界";
    std::u8string japaneseUtf8 = u8"こんにちは世界";
    std::u16string japaneseUtf16 = u"こんにちは世界";
    std::u32string japaneseUtf32 = U"こんにちは世界";

    std::string chineseNarrow = "你好世界";
    std::wstring chineseWide = L"你好世界";
    std::u8string chineseUtf8 = u8"你好世界";
    std::u16string chineseUtf16 = u"你好世界";
    std::u32string chineseUtf32 = U"你好世界";

    std::string chineseTradNarrow = "漢字測試";
    std::wstring chineseTradWide = L"漢字測試";
    std::u8string chineseTradUtf8 = u8"漢字測試";
    std::u16string chineseTradUtf16 = u"漢字測試";
    std::u32string chineseTradUtf32 = U"漢字測試";

    std::string emojiNarrow = "🌍🌟🎉";
    std::wstring emojiWide = L"🌍🌟🎉";
    std::u8string emojiUtf8 = u8"🌍🌟🎉";
    std::u16string emojiUtf16 = u"🌍🌟🎉";
    std::u32string emojiUtf32 = U"🌍🌟🎉";

    std::string mixedNarrow = "Hello 你好 こんにちは 🌍 Café";
    std::wstring mixedWide = L"Hello 你好 こんにちは 🌍 Café";
    std::u8string mixedUtf8 = u8"Hello 你好 こんにちは 🌍 Café";
    std::u16string mixedUtf16 = u"Hello 你好 こんにちは 🌍 Café";
    std::u32string mixedUtf32 = U"Hello 你好 こんにちは 🌍 Café";

    std::wstring specialWide = L"\n\t\r\\\"'";
};

template <typename ToWideFunction>
void ExpectInvalidUtf8IsRejectedOrReplaced(ToWideFunction&& toWide, const std::string& invalidUtf8)
{
    const auto w = toWide(invalidUtf8);
    // Different backends/configurations either return empty or insert replacement characters.
    EXPECT_TRUE(w.empty() || (w.find(static_cast<wchar_t>(0xFFFD)) != std::wstring::npos));
}
} // namespace

TEST(UTFConv, ToUTF8)
{
    const UTFConvTestStrings t;

    EXPECT_EQ(UTFConv::ToUTF8(t.asciiWide), t.asciiNarrow);
    EXPECT_EQ(UTFConv::ToUTF8(t.latinWide), t.latinNarrow);
    EXPECT_EQ(UTFConv::ToUTF8(t.japaneseWide), t.japaneseNarrow);
    EXPECT_EQ(UTFConv::ToUTF8(t.chineseWide), t.chineseNarrow);
    EXPECT_EQ(UTFConv::ToUTF8(t.chineseTradWide), t.chineseTradNarrow);
    EXPECT_EQ(UTFConv::ToUTF8(t.emojiWide), t.emojiNarrow);
    EXPECT_EQ(UTFConv::ToUTF8(t.mixedWide), t.mixedNarrow);

    EXPECT_EQ(UTFConv::ToUTF8(t.asciiUtf8), t.asciiNarrow);
    EXPECT_EQ(UTFConv::ToUTF8(t.asciiUtf16), t.asciiNarrow);
    EXPECT_EQ(UTFConv::ToUTF8(t.asciiUtf32), t.asciiNarrow);
}

TEST(UTFConv, ToWide)
{
    const UTFConvTestStrings t;

    EXPECT_EQ(UTFConv::ToWide(t.asciiNarrow), t.asciiWide);
    EXPECT_EQ(UTFConv::ToWide(t.latinNarrow), t.latinWide);
    EXPECT_EQ(UTFConv::ToWide(t.japaneseNarrow), t.japaneseWide);
    EXPECT_EQ(UTFConv::ToWide(t.chineseNarrow), t.chineseWide);
    EXPECT_EQ(UTFConv::ToWide(t.chineseTradNarrow), t.chineseTradWide);
    EXPECT_EQ(UTFConv::ToWide(t.emojiNarrow), t.emojiWide);
    EXPECT_EQ(UTFConv::ToWide(t.mixedNarrow), t.mixedWide);

    EXPECT_EQ(UTFConv::ToWide(t.asciiUtf8), t.asciiWide);
    EXPECT_EQ(UTFConv::ToWide(cstring_view("Hello, World!")), t.asciiWide);
    EXPECT_TRUE(UTFConv::ToWide(cstring_view("")).empty());
}

TEST(UTFConv, ToUTF16AndToUTF32)
{
    const UTFConvTestStrings t;

    EXPECT_EQ(UTFConv::ToUTF16(t.asciiWide), t.asciiUtf16);
    EXPECT_EQ(UTFConv::ToUTF16(t.mixedNarrow), t.mixedUtf16);
    EXPECT_EQ(UTFConv::ToUTF32(t.asciiWide), t.asciiUtf32);
    EXPECT_EQ(UTFConv::ToUTF32(t.mixedUtf8), t.mixedUtf32);
}

TEST(UTFConv, ViewsAndSubstrings)
{
    const UTFConvTestStrings t;

    EXPECT_EQ(UTFConv::ToUTF8(std::wstring_view(t.asciiWide)), t.asciiNarrow);
    EXPECT_EQ(UTFConv::ToUTF8(std::wstring_view(t.chineseWide)), t.chineseNarrow);

    {
        std::wstring testStr = L"Hello, World!";
        std::wstring_view view(testStr.data() + 7, 5); // "World"
        EXPECT_EQ(UTFConv::ToUTF8(view), "World");
    }
    {
        std::wstring testStr = L"你好世界";
        std::wstring_view view(testStr.data() + 2, 2); // "世界"
        EXPECT_EQ(UTFConv::ToUTF8(view), "世界");
    }
}

TEST(UTFConv, RoundTrips)
{
    const UTFConvTestStrings t;

    EXPECT_EQ(UTFConv::ToWide(UTFConv::ToUTF8(t.mixedWide)), t.mixedWide);
    EXPECT_EQ(UTFConv::ToWide(UTFConv::ToUTF8Char8(t.mixedWide)), t.mixedWide);

    EXPECT_EQ(UTFConv::ToUTF8(UTFConv::ToWide(t.mixedNarrow)), t.mixedNarrow);
    EXPECT_U8STRING_EQ(UTFConv::ToUTF8Char8(UTFConv::ToWide(t.mixedNarrow)), t.mixedUtf8);

    // UTF8Char8 -> UTF16 -> UTF32 -> UTF8Char8.
    {
        const auto utf16Result = UTFConv::ToUTF16(t.mixedUtf8);
        const auto utf32Result = UTFConv::ToUTF32(utf16Result);
        const auto utf8Result = UTFConv::ToUTF8Char8(utf32Result);
        EXPECT_U8STRING_EQ(utf8Result, t.mixedUtf8);
    }

    // Special characters.
    EXPECT_EQ(UTFConv::ToWide(UTFConv::ToUTF8(t.specialWide)), t.specialWide);
    EXPECT_EQ(UTFConv::ToWide(UTFConv::ToUTF8Char8(t.specialWide)), t.specialWide);
}

TEST(UTFConv, EmbeddedNulls)
{
    // std::string/std::wstring conversions should preserve embedded NULs (size-based inputs).
    {
        const std::string narrowWithNull("Hi\0There", 8);
        const auto wide = UTFConv::ToWide(narrowWithNull);
        EXPECT_EQ(wide.size(), 8u);
        EXPECT_EQ(wide[2], L'\0');
        EXPECT_EQ(UTFConv::ToUTF8(wide), narrowWithNull);
    }
    {
        std::wstring wideWithNull = L"Hello\0World";
        wideWithNull.resize(11);
        const auto utf8 = UTFConv::ToUTF8(wideWithNull);
        EXPECT_EQ(utf8.size(), 11u);
        EXPECT_EQ(utf8[5], '\0');
    }

    // cstring_view is NTBS: it stops at the first NUL.
    {
        static const char ntbsWithNull[] = "Hi\0There";
        const auto wide = UTFConv::ToWide(cstring_view(ntbsWithNull));
        EXPECT_EQ(wide, std::wstring(L"Hi"));
    }
}

TEST(UTFConv, BinaryContentMatches)
{
    const UTFConvTestStrings t;

    const auto utf8Result = UTFConv::ToUTF8(t.mixedWide);
    const auto utf8Char8Result = UTFConv::ToUTF8Char8(t.mixedWide);
    ASSERT_EQ(utf8Result.size(), utf8Char8Result.size());
    EXPECT_EQ(std::memcmp(utf8Result.data(), utf8Char8Result.data(), utf8Result.size()), 0);
}

TEST(UTFConv, EmptyInputs)
{
    EXPECT_TRUE(UTFConv::ToUTF8(std::string()).empty());
    EXPECT_TRUE(UTFConv::ToUTF8(std::wstring()).empty());
    EXPECT_TRUE(UTFConv::ToUTF8(std::u8string()).empty());
    EXPECT_TRUE(UTFConv::ToUTF8(std::u16string()).empty());
    EXPECT_TRUE(UTFConv::ToUTF8(std::u32string()).empty());

    EXPECT_TRUE(UTFConv::ToWide(std::string()).empty());
    EXPECT_TRUE(UTFConv::ToWide(std::wstring()).empty());
    EXPECT_TRUE(UTFConv::ToWide(std::u8string()).empty());
    EXPECT_TRUE(UTFConv::ToWide(std::u16string()).empty());
    EXPECT_TRUE(UTFConv::ToWide(std::u32string()).empty());

    EXPECT_TRUE(UTFConv::ToUTF8Char8(std::string()).empty());
    EXPECT_TRUE(UTFConv::ToUTF8Char8(std::wstring()).empty());
    EXPECT_TRUE(UTFConv::ToUTF16(std::string()).empty());
    EXPECT_TRUE(UTFConv::ToUTF32(std::string()).empty());

    EXPECT_TRUE(UTFConv::ToUTF8Native(std::wstring()).empty());
    EXPECT_TRUE(UTFConv::ToWideNative(std::string()).empty());
}

TEST(UTFConv, NativeFunctions)
{
    const UTFConvTestStrings t;

    EXPECT_EQ(UTFConv::ToUTF8Native(L"Hello"), "Hello");
    EXPECT_EQ(UTFConv::ToUTF8Native(L"Café"), "Café");
    EXPECT_EQ(UTFConv::ToUTF8Native(L"你好"), "你好");
    EXPECT_EQ(UTFConv::ToUTF8Native(L"こんにちは"), "こんにちは");

    EXPECT_EQ(UTFConv::ToWideNative("Hello"), std::wstring(L"Hello"));
    EXPECT_EQ(UTFConv::ToWideNative("Café"), std::wstring(L"Café"));
    EXPECT_EQ(UTFConv::ToWideNative("你好"), std::wstring(L"你好"));
    EXPECT_EQ(UTFConv::ToWideNative("こんにちは"), std::wstring(L"こんにちは"));
}

TEST(UTFConv, ExplicitSurrogatePairInputs)
{
    // U+1F30D (EARTH GLOBE EUROPE-AFRICA) in UTF-16: D83C DF0D.
    const std::u16string earthUtf16 = {0xD83C, 0xDF0D};
    const std::u32string earthUtf32 = {0x0001F30D};

    const std::string earthUtf8 = "🌍";
    const std::u8string earthUtf8c = u8"🌍";

    EXPECT_EQ(UTFConv::ToUTF8(earthUtf16), earthUtf8);
    EXPECT_EQ(UTFConv::ToUTF8(earthUtf32), earthUtf8);
    EXPECT_U8STRING_EQ(UTFConv::ToUTF8Char8(earthUtf16), earthUtf8c);
    EXPECT_U8STRING_EQ(UTFConv::ToUTF8Char8(earthUtf32), earthUtf8c);

    // Round-trip back to UTF-16/UTF-32.
    EXPECT_EQ(UTFConv::ToUTF16(earthUtf8), earthUtf16);
    EXPECT_EQ(UTFConv::ToUTF32(earthUtf8), earthUtf32);

    // Platform detail: wchar_t is UTF-16 on Windows, UTF-32 on most Unix-likes.
    const auto wide = UTFConv::ToWide(earthUtf8);
#if defined(RAD_OS_WINDOWS)
    EXPECT_EQ(wide.size(), 2u);
#else
    EXPECT_EQ(wide.size(), 1u);
#endif
}

TEST(UTFConv, InvalidUtfInputs)
{
    // Truncated UTF-8 sequence (invalid).
    const std::string invalidUtf8Truncated = std::string("\xE3\x81", 2);
    // UTF-8 encoding of a surrogate code point is invalid (ED A0 80 = U+D800).
    const std::string invalidUtf8Surrogate = std::string("\xED\xA0\x80", 3);
    // Overlong UTF-8 encoding (invalid).
    const std::string invalidUtf8Overlong = std::string("\xC0\xAF", 2);
    // Bad continuation byte (invalid).
    const std::string invalidUtf8BadContinuation = std::string("\xE2\x28\xA1", 3);

#if defined(RAD_OS_WINDOWS)
    EXPECT_TRUE(UTFConv::ToWideNative(invalidUtf8Truncated).empty());
    EXPECT_TRUE(UTFConv::ToWideNative(invalidUtf8Surrogate).empty());
    EXPECT_TRUE(UTFConv::ToWideNative(invalidUtf8Overlong).empty());
    EXPECT_TRUE(UTFConv::ToWideNative(invalidUtf8BadContinuation).empty());
#else
    ExpectInvalidUtf8IsRejectedOrReplaced(UTFConv::ToWideNative, invalidUtf8Truncated);
    ExpectInvalidUtf8IsRejectedOrReplaced(UTFConv::ToWideNative, invalidUtf8Surrogate);
    ExpectInvalidUtf8IsRejectedOrReplaced(UTFConv::ToWideNative, invalidUtf8Overlong);
    ExpectInvalidUtf8IsRejectedOrReplaced(UTFConv::ToWideNative, invalidUtf8BadContinuation);
#endif

    ExpectInvalidUtf8IsRejectedOrReplaced([](const std::string& s) { return UTFConv::ToWide(s); },
                                          invalidUtf8Truncated);
    ExpectInvalidUtf8IsRejectedOrReplaced([](const std::string& s) { return UTFConv::ToWide(s); },
                                          invalidUtf8Surrogate);
    ExpectInvalidUtf8IsRejectedOrReplaced([](const std::string& s) { return UTFConv::ToWide(s); },
                                          invalidUtf8Overlong);
    ExpectInvalidUtf8IsRejectedOrReplaced(
        [](const std::string& s) { return UTFConv::ToWide(s); }, invalidUtf8BadContinuation);

    // Lone surrogate in UTF-16 input should be rejected by strict native conversion on Windows.
    std::wstring loneHighSurrogate(1, static_cast<wchar_t>(0xD800));
#if defined(RAD_OS_WINDOWS)
    EXPECT_TRUE(UTFConv::ToUTF8Native(loneHighSurrogate).empty());
#else
    const auto utf8 = UTFConv::ToUTF8Native(loneHighSurrogate);
    EXPECT_FALSE(utf8.empty());
#endif
}
