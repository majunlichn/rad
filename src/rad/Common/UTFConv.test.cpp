#include <rad/Common/UTFConv.h>

#include <gtest/gtest.h>

#include <string>
#include <string_view>

using namespace rad;

// Helper to compare std::u8string without using EXPECT_EQ directly
#define EXPECT_U8STRING_EQ(val1, val2) EXPECT_EQ(UTFConv::ToUTF8(val1), UTFConv::ToUTF8(val2))

TEST(Common, UTFConv)
{
    // Test strings
    // ASCII
    std::string asciiNarrow = "Hello, World!";
    std::wstring asciiWide = L"Hello, World!";
    std::u8string asciiUtf8 = u8"Hello, World!";
    std::u16string asciiUtf16 = u"Hello, World!";
    std::u32string asciiUtf32 = U"Hello, World!";

    // Unicode with Latin accents
    std::string latinNarrow = "Café résumé naïve";
    std::wstring latinWide = L"Café résumé naïve";
    std::u8string latinUtf8 = u8"Café résumé naïve";
    std::u16string latinUtf16 = u"Café résumé naïve";
    std::u32string latinUtf32 = U"Café résumé naïve";

    // Japanese
    std::string japaneseNarrow = "こんにちは世界";
    std::wstring japaneseWide = L"こんにちは世界";
    std::u8string japaneseUtf8 = u8"こんにちは世界";
    std::u16string japaneseUtf16 = u"こんにちは世界";
    std::u32string japaneseUtf32 = U"こんにちは世界";

    // Chinese (Simplified)
    std::string chineseNarrow = "你好世界";
    std::wstring chineseWide = L"你好世界";
    std::u8string chineseUtf8 = u8"你好世界";
    std::u16string chineseUtf16 = u"你好世界";
    std::u32string chineseUtf32 = U"你好世界";

    // Chinese (Traditional)
    std::string chineseTradNarrow = "漢字測試";
    std::wstring chineseTradWide = L"漢字測試";
    std::u8string chineseTradUtf8 = u8"漢字測試";
    std::u16string chineseTradUtf16 = u"漢字測試";
    std::u32string chineseTradUtf32 = U"漢字測試";

    // Emoji
    std::string emojiNarrow = "🌍🌟🎉";
    std::wstring emojiWide = L"🌍🌟🎉";
    std::u8string emojiUtf8 = u8"🌍🌟🎉";
    std::u16string emojiUtf16 = u"🌍🌟🎉";
    std::u32string emojiUtf32 = U"🌍🌟🎉";

    // Mixed scripts
    std::string mixedNarrow = "Hello 你好 こんにちは 🌍 Café";
    std::wstring mixedWide = L"Hello 你好 こんにちは 🌍 Café";
    std::u8string mixedUtf8 = u8"Hello 你好 こんにちは 🌍 Café";
    std::u16string mixedUtf16 = u"Hello 你好 こんにちは 🌍 Café";
    std::u32string mixedUtf32 = U"Hello 你好 こんにちは 🌍 Café";

    // Empty strings
    std::string emptyNarrow;
    std::wstring emptyWide;
    std::u8string emptyUtf8;
    std::u16string emptyUtf16;
    std::u32string emptyUtf32;

    // Special characters
    std::wstring specialWide = L"\n\t\r\\\"'";

    // =========================================================================
    // ToUTF8 Tests (returns std::string)
    // =========================================================================

    // From wstring
    EXPECT_EQ(UTFConv::ToUTF8(asciiWide), asciiNarrow);
    EXPECT_EQ(UTFConv::ToUTF8(latinWide), latinNarrow);
    EXPECT_EQ(UTFConv::ToUTF8(japaneseWide), japaneseNarrow);
    EXPECT_EQ(UTFConv::ToUTF8(chineseWide), chineseNarrow);
    EXPECT_EQ(UTFConv::ToUTF8(chineseTradWide), chineseTradNarrow);
    EXPECT_EQ(UTFConv::ToUTF8(emojiWide), emojiNarrow);
    EXPECT_EQ(UTFConv::ToUTF8(mixedWide), mixedNarrow);
    EXPECT_TRUE(UTFConv::ToUTF8(emptyWide).empty());

    // From wstring_view
    EXPECT_EQ(UTFConv::ToUTF8(std::wstring_view(asciiWide)), asciiNarrow);
    EXPECT_EQ(UTFConv::ToUTF8(std::wstring_view(chineseWide)), chineseNarrow);

    // From wstring_view substring
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

    // From u8string
    EXPECT_EQ(UTFConv::ToUTF8(asciiUtf8), asciiNarrow);
    EXPECT_EQ(UTFConv::ToUTF8(chineseUtf8), chineseNarrow);
    EXPECT_EQ(UTFConv::ToUTF8(emojiUtf8), emojiNarrow);
    EXPECT_EQ(UTFConv::ToUTF8(mixedUtf8), mixedNarrow);

    // From u16string
    EXPECT_EQ(UTFConv::ToUTF8(asciiUtf16), asciiNarrow);
    EXPECT_EQ(UTFConv::ToUTF8(latinUtf16), latinNarrow);
    EXPECT_EQ(UTFConv::ToUTF8(japaneseUtf16), japaneseNarrow);
    EXPECT_EQ(UTFConv::ToUTF8(chineseUtf16), chineseNarrow);
    EXPECT_EQ(UTFConv::ToUTF8(emojiUtf16), emojiNarrow);
    EXPECT_EQ(UTFConv::ToUTF8(mixedUtf16), mixedNarrow);

    // From u32string
    EXPECT_EQ(UTFConv::ToUTF8(asciiUtf32), asciiNarrow);
    EXPECT_EQ(UTFConv::ToUTF8(latinUtf32), latinNarrow);
    EXPECT_EQ(UTFConv::ToUTF8(japaneseUtf32), japaneseNarrow);
    EXPECT_EQ(UTFConv::ToUTF8(chineseUtf32), chineseNarrow);
    EXPECT_EQ(UTFConv::ToUTF8(chineseTradUtf32), chineseTradNarrow);
    EXPECT_EQ(UTFConv::ToUTF8(emojiUtf32), emojiNarrow);
    EXPECT_EQ(UTFConv::ToUTF8(mixedUtf32), mixedNarrow);

    // =========================================================================
    // ToWide Tests (returns std::wstring)
    // =========================================================================

    // From narrow string
    EXPECT_EQ(UTFConv::ToWide(asciiNarrow), asciiWide);
    EXPECT_EQ(UTFConv::ToWide(latinNarrow), latinWide);
    EXPECT_EQ(UTFConv::ToWide(japaneseNarrow), japaneseWide);
    EXPECT_EQ(UTFConv::ToWide(chineseNarrow), chineseWide);
    EXPECT_EQ(UTFConv::ToWide(chineseTradNarrow), chineseTradWide);
    EXPECT_EQ(UTFConv::ToWide(emojiNarrow), emojiWide);
    EXPECT_EQ(UTFConv::ToWide(mixedNarrow), mixedWide);
    EXPECT_TRUE(UTFConv::ToWide(emptyNarrow).empty());

    // From u8string
    EXPECT_EQ(UTFConv::ToWide(asciiUtf8), asciiWide);
    EXPECT_EQ(UTFConv::ToWide(latinUtf8), latinWide);
    EXPECT_EQ(UTFConv::ToWide(japaneseUtf8), japaneseWide);
    EXPECT_EQ(UTFConv::ToWide(chineseUtf8), chineseWide);
    EXPECT_EQ(UTFConv::ToWide(chineseTradUtf8), chineseTradWide);
    EXPECT_EQ(UTFConv::ToWide(emojiUtf8), emojiWide);
    EXPECT_EQ(UTFConv::ToWide(mixedUtf8), mixedWide);
    EXPECT_TRUE(UTFConv::ToWide(emptyUtf8).empty());

    // From cstring_view
    {
        std::string str = "Hello, World!";
        cstring_view view(str);
        EXPECT_EQ(UTFConv::ToWide(view), asciiWide);
    }
    {
        std::string str = "你好世界";
        cstring_view view(str);
        EXPECT_EQ(UTFConv::ToWide(view), chineseWide);
    }
    {
        std::string str = "Café résumé naïve";
        cstring_view view(str);
        EXPECT_EQ(UTFConv::ToWide(view), latinWide);
    }
    {
        std::string str;
        cstring_view view(str);
        EXPECT_TRUE(UTFConv::ToWide(view).empty());
    }

    // =========================================================================
    // ToUTF8Char8 Tests (returns std::u8string)
    // =========================================================================

    // From wstring - compare via ToUTF8 conversion
    EXPECT_U8STRING_EQ(UTFConv::ToUTF8Char8(asciiWide), asciiUtf8);
    EXPECT_U8STRING_EQ(UTFConv::ToUTF8Char8(latinWide), latinUtf8);
    EXPECT_U8STRING_EQ(UTFConv::ToUTF8Char8(japaneseWide), japaneseUtf8);
    EXPECT_U8STRING_EQ(UTFConv::ToUTF8Char8(chineseWide), chineseUtf8);
    EXPECT_U8STRING_EQ(UTFConv::ToUTF8Char8(chineseTradWide), chineseTradUtf8);
    EXPECT_U8STRING_EQ(UTFConv::ToUTF8Char8(emojiWide), emojiUtf8);
    EXPECT_U8STRING_EQ(UTFConv::ToUTF8Char8(mixedWide), mixedUtf8);
    EXPECT_TRUE(UTFConv::ToUTF8Char8(emptyWide).empty());

    // From narrow string
    EXPECT_U8STRING_EQ(UTFConv::ToUTF8Char8(asciiNarrow), asciiUtf8);
    EXPECT_U8STRING_EQ(UTFConv::ToUTF8Char8(latinNarrow), latinUtf8);
    EXPECT_U8STRING_EQ(UTFConv::ToUTF8Char8(japaneseNarrow), japaneseUtf8);
    EXPECT_U8STRING_EQ(UTFConv::ToUTF8Char8(chineseNarrow), chineseUtf8);
    EXPECT_U8STRING_EQ(UTFConv::ToUTF8Char8(chineseTradNarrow), chineseTradUtf8);
    EXPECT_U8STRING_EQ(UTFConv::ToUTF8Char8(emojiNarrow), emojiUtf8);
    EXPECT_U8STRING_EQ(UTFConv::ToUTF8Char8(mixedNarrow), mixedUtf8);
    EXPECT_TRUE(UTFConv::ToUTF8Char8(emptyNarrow).empty());

    // From u16string
    EXPECT_U8STRING_EQ(UTFConv::ToUTF8Char8(asciiUtf16), asciiUtf8);
    EXPECT_U8STRING_EQ(UTFConv::ToUTF8Char8(latinUtf16), latinUtf8);
    EXPECT_U8STRING_EQ(UTFConv::ToUTF8Char8(japaneseUtf16), japaneseUtf8);
    EXPECT_U8STRING_EQ(UTFConv::ToUTF8Char8(chineseUtf16), chineseUtf8);
    EXPECT_U8STRING_EQ(UTFConv::ToUTF8Char8(chineseTradUtf16), chineseTradUtf8);
    EXPECT_U8STRING_EQ(UTFConv::ToUTF8Char8(emojiUtf16), emojiUtf8);
    EXPECT_U8STRING_EQ(UTFConv::ToUTF8Char8(mixedUtf16), mixedUtf8);
    EXPECT_TRUE(UTFConv::ToUTF8Char8(emptyUtf16).empty());

    // From u32string
    EXPECT_U8STRING_EQ(UTFConv::ToUTF8Char8(asciiUtf32), asciiUtf8);
    EXPECT_U8STRING_EQ(UTFConv::ToUTF8Char8(latinUtf32), latinUtf8);
    EXPECT_U8STRING_EQ(UTFConv::ToUTF8Char8(japaneseUtf32), japaneseUtf8);
    EXPECT_U8STRING_EQ(UTFConv::ToUTF8Char8(chineseUtf32), chineseUtf8);
    EXPECT_U8STRING_EQ(UTFConv::ToUTF8Char8(chineseTradUtf32), chineseTradUtf8);
    EXPECT_U8STRING_EQ(UTFConv::ToUTF8Char8(emojiUtf32), emojiUtf8);
    EXPECT_U8STRING_EQ(UTFConv::ToUTF8Char8(mixedUtf32), mixedUtf8);
    EXPECT_TRUE(UTFConv::ToUTF8Char8(emptyUtf32).empty());

    // =========================================================================
    // ToUTF16 Tests (returns std::u16string)
    // =========================================================================

    // From wstring
    EXPECT_EQ(UTFConv::ToUTF16(asciiWide), asciiUtf16);
    EXPECT_EQ(UTFConv::ToUTF16(latinWide), latinUtf16);
    EXPECT_EQ(UTFConv::ToUTF16(japaneseWide), japaneseUtf16);
    EXPECT_EQ(UTFConv::ToUTF16(chineseWide), chineseUtf16);
    EXPECT_EQ(UTFConv::ToUTF16(chineseTradWide), chineseTradUtf16);
    EXPECT_EQ(UTFConv::ToUTF16(emojiWide), emojiUtf16);
    EXPECT_EQ(UTFConv::ToUTF16(mixedWide), mixedUtf16);
    EXPECT_TRUE(UTFConv::ToUTF16(emptyWide).empty());

    // From narrow string
    EXPECT_EQ(UTFConv::ToUTF16(asciiNarrow), asciiUtf16);
    EXPECT_EQ(UTFConv::ToUTF16(latinNarrow), latinUtf16);
    EXPECT_EQ(UTFConv::ToUTF16(japaneseNarrow), japaneseUtf16);
    EXPECT_EQ(UTFConv::ToUTF16(chineseNarrow), chineseUtf16);
    EXPECT_EQ(UTFConv::ToUTF16(chineseTradNarrow), chineseTradUtf16);
    EXPECT_EQ(UTFConv::ToUTF16(emojiNarrow), emojiUtf16);
    EXPECT_EQ(UTFConv::ToUTF16(mixedNarrow), mixedUtf16);
    EXPECT_TRUE(UTFConv::ToUTF16(emptyNarrow).empty());

    // From u8string
    EXPECT_EQ(UTFConv::ToUTF16(asciiUtf8), asciiUtf16);
    EXPECT_EQ(UTFConv::ToUTF16(latinUtf8), latinUtf16);
    EXPECT_EQ(UTFConv::ToUTF16(japaneseUtf8), japaneseUtf16);
    EXPECT_EQ(UTFConv::ToUTF16(chineseUtf8), chineseUtf16);
    EXPECT_EQ(UTFConv::ToUTF16(chineseTradUtf8), chineseTradUtf16);
    EXPECT_EQ(UTFConv::ToUTF16(emojiUtf8), emojiUtf16);
    EXPECT_EQ(UTFConv::ToUTF16(mixedUtf8), mixedUtf16);
    EXPECT_TRUE(UTFConv::ToUTF16(emptyUtf8).empty());

    // From u32string
    EXPECT_EQ(UTFConv::ToUTF16(asciiUtf32), asciiUtf16);
    EXPECT_EQ(UTFConv::ToUTF16(latinUtf32), latinUtf16);
    EXPECT_EQ(UTFConv::ToUTF16(japaneseUtf32), japaneseUtf16);
    EXPECT_EQ(UTFConv::ToUTF16(chineseUtf32), chineseUtf16);
    EXPECT_EQ(UTFConv::ToUTF16(chineseTradUtf32), chineseTradUtf16);
    EXPECT_EQ(UTFConv::ToUTF16(emojiUtf32), emojiUtf16);
    EXPECT_EQ(UTFConv::ToUTF16(mixedUtf32), mixedUtf16);
    EXPECT_TRUE(UTFConv::ToUTF16(emptyUtf32).empty());

    // =========================================================================
    // ToUTF32 Tests (returns std::u32string)
    // =========================================================================

    // From wstring
    EXPECT_EQ(UTFConv::ToUTF32(asciiWide), asciiUtf32);
    EXPECT_EQ(UTFConv::ToUTF32(latinWide), latinUtf32);
    EXPECT_EQ(UTFConv::ToUTF32(japaneseWide), japaneseUtf32);
    EXPECT_EQ(UTFConv::ToUTF32(chineseWide), chineseUtf32);
    EXPECT_EQ(UTFConv::ToUTF32(chineseTradWide), chineseTradUtf32);
    EXPECT_EQ(UTFConv::ToUTF32(emojiWide), emojiUtf32);
    EXPECT_EQ(UTFConv::ToUTF32(mixedWide), mixedUtf32);
    EXPECT_TRUE(UTFConv::ToUTF32(emptyWide).empty());

    // From narrow string
    EXPECT_EQ(UTFConv::ToUTF32(asciiNarrow), asciiUtf32);
    EXPECT_EQ(UTFConv::ToUTF32(latinNarrow), latinUtf32);
    EXPECT_EQ(UTFConv::ToUTF32(japaneseNarrow), japaneseUtf32);
    EXPECT_EQ(UTFConv::ToUTF32(chineseNarrow), chineseUtf32);
    EXPECT_EQ(UTFConv::ToUTF32(chineseTradNarrow), chineseTradUtf32);
    EXPECT_EQ(UTFConv::ToUTF32(emojiNarrow), emojiUtf32);
    EXPECT_EQ(UTFConv::ToUTF32(mixedNarrow), mixedUtf32);
    EXPECT_TRUE(UTFConv::ToUTF32(emptyNarrow).empty());

    // From u8string
    EXPECT_EQ(UTFConv::ToUTF32(asciiUtf8), asciiUtf32);
    EXPECT_EQ(UTFConv::ToUTF32(latinUtf8), latinUtf32);
    EXPECT_EQ(UTFConv::ToUTF32(japaneseUtf8), japaneseUtf32);
    EXPECT_EQ(UTFConv::ToUTF32(chineseUtf8), chineseUtf32);
    EXPECT_EQ(UTFConv::ToUTF32(chineseTradUtf8), chineseTradUtf32);
    EXPECT_EQ(UTFConv::ToUTF32(emojiUtf8), emojiUtf32);
    EXPECT_EQ(UTFConv::ToUTF32(mixedUtf8), mixedUtf32);
    EXPECT_TRUE(UTFConv::ToUTF32(emptyUtf8).empty());

    // From u16string
    EXPECT_EQ(UTFConv::ToUTF32(asciiUtf16), asciiUtf32);
    EXPECT_EQ(UTFConv::ToUTF32(latinUtf16), latinUtf32);
    EXPECT_EQ(UTFConv::ToUTF32(japaneseUtf16), japaneseUtf32);
    EXPECT_EQ(UTFConv::ToUTF32(chineseUtf16), chineseUtf32);
    EXPECT_EQ(UTFConv::ToUTF32(chineseTradUtf16), chineseTradUtf32);
    EXPECT_EQ(UTFConv::ToUTF32(emojiUtf16), emojiUtf32);
    EXPECT_EQ(UTFConv::ToUTF32(mixedUtf16), mixedUtf32);
    EXPECT_TRUE(UTFConv::ToUTF32(emptyUtf16).empty());

    // =========================================================================
    // Round-Trip Conversion Tests
    // =========================================================================

    // Wide -> UTF8 -> Wide
    EXPECT_EQ(UTFConv::ToWide(UTFConv::ToUTF8(asciiWide)), asciiWide);
    EXPECT_EQ(UTFConv::ToWide(UTFConv::ToUTF8(latinWide)), latinWide);
    EXPECT_EQ(UTFConv::ToWide(UTFConv::ToUTF8(japaneseWide)), japaneseWide);
    EXPECT_EQ(UTFConv::ToWide(UTFConv::ToUTF8(chineseWide)), chineseWide);
    EXPECT_EQ(UTFConv::ToWide(UTFConv::ToUTF8(chineseTradWide)), chineseTradWide);
    EXPECT_EQ(UTFConv::ToWide(UTFConv::ToUTF8(emojiWide)), emojiWide);
    EXPECT_EQ(UTFConv::ToWide(UTFConv::ToUTF8(mixedWide)), mixedWide);

    // Wide -> UTF8Char8 -> Wide
    EXPECT_EQ(UTFConv::ToWide(UTFConv::ToUTF8Char8(asciiWide)), asciiWide);
    EXPECT_EQ(UTFConv::ToWide(UTFConv::ToUTF8Char8(latinWide)), latinWide);
    EXPECT_EQ(UTFConv::ToWide(UTFConv::ToUTF8Char8(japaneseWide)), japaneseWide);
    EXPECT_EQ(UTFConv::ToWide(UTFConv::ToUTF8Char8(chineseWide)), chineseWide);
    EXPECT_EQ(UTFConv::ToWide(UTFConv::ToUTF8Char8(emojiWide)), emojiWide);
    EXPECT_EQ(UTFConv::ToWide(UTFConv::ToUTF8Char8(mixedWide)), mixedWide);

    // Narrow -> Wide -> UTF8
    EXPECT_EQ(UTFConv::ToUTF8(UTFConv::ToWide(asciiNarrow)), asciiNarrow);
    EXPECT_EQ(UTFConv::ToUTF8(UTFConv::ToWide(latinNarrow)), latinNarrow);
    EXPECT_EQ(UTFConv::ToUTF8(UTFConv::ToWide(japaneseNarrow)), japaneseNarrow);
    EXPECT_EQ(UTFConv::ToUTF8(UTFConv::ToWide(chineseNarrow)), chineseNarrow);
    EXPECT_EQ(UTFConv::ToUTF8(UTFConv::ToWide(emojiNarrow)), emojiNarrow);
    EXPECT_EQ(UTFConv::ToUTF8(UTFConv::ToWide(mixedNarrow)), mixedNarrow);

    // Narrow -> Wide -> UTF8Char8
    EXPECT_U8STRING_EQ(UTFConv::ToUTF8Char8(UTFConv::ToWide(asciiNarrow)), asciiUtf8);
    EXPECT_U8STRING_EQ(UTFConv::ToUTF8Char8(UTFConv::ToWide(latinNarrow)), latinUtf8);
    EXPECT_U8STRING_EQ(UTFConv::ToUTF8Char8(UTFConv::ToWide(japaneseNarrow)), japaneseUtf8);
    EXPECT_U8STRING_EQ(UTFConv::ToUTF8Char8(UTFConv::ToWide(chineseNarrow)), chineseUtf8);
    EXPECT_U8STRING_EQ(UTFConv::ToUTF8Char8(UTFConv::ToWide(emojiNarrow)), emojiUtf8);
    EXPECT_U8STRING_EQ(UTFConv::ToUTF8Char8(UTFConv::ToWide(mixedNarrow)), mixedUtf8);

    // All formats round-trip: UTF8Char8 -> UTF16 -> UTF32 -> UTF8Char8
    {
        auto utf16Result = UTFConv::ToUTF16(mixedUtf8);
        auto utf32Result = UTFConv::ToUTF32(utf16Result);
        auto utf8Result = UTFConv::ToUTF8Char8(utf32Result);
        EXPECT_U8STRING_EQ(utf8Result, mixedUtf8);
    }

    // All formats round-trip: UTF8 -> UTF16 -> UTF32 -> UTF8
    {
        auto utf16Result = UTFConv::ToUTF16(mixedNarrow);
        auto utf32Result = UTFConv::ToUTF32(utf16Result);
        auto utf8Result = UTFConv::ToUTF8(utf32Result);
        EXPECT_EQ(utf8Result, mixedNarrow);
    }

    // Special characters round-trip
    EXPECT_EQ(UTFConv::ToWide(UTFConv::ToUTF8(specialWide)), specialWide);
    EXPECT_EQ(UTFConv::ToWide(UTFConv::ToUTF8Char8(specialWide)), specialWide);

    // =========================================================================
    // Edge Case Tests
    // =========================================================================

    // Single character - ASCII
    EXPECT_EQ(UTFConv::ToUTF8(std::wstring(L"A")), "A");
    EXPECT_U8STRING_EQ(UTFConv::ToUTF8Char8(std::wstring(L"A")), u8"A");

    // Single character - Latin accent (fits in single wchar_t on all platforms)
    EXPECT_EQ(UTFConv::ToUTF8(std::wstring(L"é")), "é");
    EXPECT_U8STRING_EQ(UTFConv::ToUTF8Char8(std::wstring(L"é")), u8"é");

    // Single character - CJK (fits in single wchar_t on all platforms)
    EXPECT_EQ(UTFConv::ToUTF8(std::wstring(L"世")), "世");
    EXPECT_U8STRING_EQ(UTFConv::ToUTF8Char8(std::wstring(L"世")), u8"世");

    // Single emoji - use string literal instead of character literal
    {
        std::wstring emojiStr = L"🌍"; // May be 2 wchar_t on Windows
        EXPECT_EQ(UTFConv::ToUTF8(emojiStr), "🌍");
        EXPECT_U8STRING_EQ(UTFConv::ToUTF8Char8(emojiStr), u8"🌍");
        EXPECT_EQ(UTFConv::ToUTF8(emojiStr).size(), 4u);
        EXPECT_EQ(UTFConv::ToUTF8Char8(emojiStr).size(), 4u);
    }

    // Single character size verification
    EXPECT_EQ(UTFConv::ToUTF8(std::wstring(L"A")).size(), 1u);
    EXPECT_EQ(UTFConv::ToUTF8(std::wstring(L"é")).size(), 2u);
    EXPECT_EQ(UTFConv::ToUTF8(std::wstring(L"世")).size(), 3u);

    EXPECT_EQ(UTFConv::ToUTF8Char8(std::wstring(L"A")).size(), 1u);
    EXPECT_EQ(UTFConv::ToUTF8Char8(std::wstring(L"é")).size(), 2u);
    EXPECT_EQ(UTFConv::ToUTF8Char8(std::wstring(L"世")).size(), 3u);

    // Null character in middle of string
    {
        std::wstring strWithNull = L"Hello\0World";
        strWithNull.resize(11);
        auto result = UTFConv::ToUTF8(strWithNull);
        EXPECT_EQ(result.size(), 11u);
        EXPECT_EQ(result[5], '\0');
    }
    {
        std::wstring strWithNull = L"Hello\0World";
        strWithNull.resize(11);
        auto result = UTFConv::ToUTF8Char8(strWithNull);
        EXPECT_EQ(result.size(), 11u);
        EXPECT_EQ(static_cast<char>(result[5]), '\0');
    }

    // Very long string
    {
        std::wstring longStr(10000, L'A');
        auto result = UTFConv::ToUTF8(longStr);
        EXPECT_EQ(result.size(), 10000u);
        EXPECT_EQ(result, std::string(10000, 'A'));
    }
    {
        std::wstring longStr(10000, L'A');
        auto result = UTFConv::ToUTF8Char8(longStr);
        EXPECT_EQ(result.size(), 10000u);
        EXPECT_U8STRING_EQ(result, std::u8string(10000, u8'A'));
    }

    // =========================================================================
    // Binary Content Verification Tests
    // =========================================================================

    // ToUTF8 and ToUTF8Char8 produce identical binary content
    {
        auto utf8Result = UTFConv::ToUTF8(chineseWide);
        auto utf8Char8Result = UTFConv::ToUTF8Char8(chineseWide);
        EXPECT_EQ(utf8Result.size(), utf8Char8Result.size());
        EXPECT_EQ(memcmp(utf8Result.data(), utf8Char8Result.data(), utf8Result.size()), 0);
    }
    {
        auto utf8Result = UTFConv::ToUTF8(mixedWide);
        auto utf8Char8Result = UTFConv::ToUTF8Char8(mixedWide);
        EXPECT_EQ(utf8Result.size(), utf8Char8Result.size());
        EXPECT_EQ(memcmp(utf8Result.data(), utf8Char8Result.data(), utf8Result.size()), 0);
    }
    {
        std::wstring emojiStr = L"🌍🌟🎉";
        auto utf8Result = UTFConv::ToUTF8(emojiStr);
        auto utf8Char8Result = UTFConv::ToUTF8Char8(emojiStr);
        EXPECT_EQ(utf8Result.size(), utf8Char8Result.size());
        EXPECT_EQ(memcmp(utf8Result.data(), utf8Char8Result.data(), utf8Result.size()), 0);
    }

    // =========================================================================
    // Native Function Tests
    // =========================================================================

    // ToUTF8Native
    {
        auto result = UTFConv::ToUTF8Native(std::wstring_view(L"Hello"));
        EXPECT_FALSE(result.empty());
        EXPECT_EQ(result, "Hello");
    }
    {
        auto result = UTFConv::ToUTF8Native(std::wstring_view(L"Café"));
        EXPECT_FALSE(result.empty());
        EXPECT_EQ(result, "Café");
    }
    {
        std::wstring str = L"你好";
        auto result = UTFConv::ToUTF8Native(std::wstring_view(str));
        EXPECT_FALSE(result.empty());
        EXPECT_EQ(result, "你好");
    }
    {
        std::wstring str = L"こんにちは";
        auto result = UTFConv::ToUTF8Native(std::wstring_view(str));
        EXPECT_FALSE(result.empty());
        EXPECT_EQ(result, "こんにちは");
    }

    // ToWideNative
    {
        auto result = UTFConv::ToWideNative(std::string_view("Hello"));
        EXPECT_FALSE(result.empty());
        EXPECT_EQ(result, std::wstring(L"Hello"));
    }
    {
        auto result = UTFConv::ToWideNative(std::string_view("Café"));
        EXPECT_FALSE(result.empty());
        EXPECT_EQ(result, std::wstring(L"Café"));
    }
    {
        auto result = UTFConv::ToWideNative(std::string_view("你好"));
        EXPECT_FALSE(result.empty());
        EXPECT_EQ(result, std::wstring(L"你好"));
    }
    {
        auto result = UTFConv::ToWideNative(std::string_view("こんにちは"));
        EXPECT_FALSE(result.empty());
        EXPECT_EQ(result, std::wstring(L"こんにちは"));
    }
}
