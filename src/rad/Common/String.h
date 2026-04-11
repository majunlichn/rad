#pragma once

#include <rad/Common/Platform.h>

#include <pystring/pystring.h>

#include <algorithm>
#include <cassert>
#include <cctype>
#include <format>
#include <ranges>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include <regex>
#include <sstream>

#include <beman/cstring_view/cstring_view.hpp>

#if defined(RAD_COMPILER_MSVC)
#if !defined(strcasecmp)
#define strcasecmp(a, b) _stricmp(a, b)
#endif
#if !defined(strncasecmp)
#define strncasecmp(a, b, n) _strnicmp(a, b, n)
#endif
#else
#include <strings.h>
#endif

namespace rad
{

// std::string is expected to be UTF-8 encoded and null-terminated.

// Non-owning view of a null-terminated byte string (NTBS).
using cstring_view = beman::cstring_view;

// Throws std::runtime_error with format error.
std::string StrPrintf(cstring_view format, ...);
// Throws std::runtime_error with format error.
std::string StrPrintfV(cstring_view format, va_list args);

bool StrEqual(std::string_view str1, std::string_view str2);
bool StrCaseEqual(std::string_view str1, std::string_view str2);

std::string StrUpper(std::string_view s);
std::string StrLower(std::string_view s);
void StrUpperInPlace(std::string& s);
void StrLowerInPlace(std::string& s);

std::string StrTrim(std::string_view str, std::string_view charlist = " \t\n\v\f\r");
void StrTrimInPlace(std::string& str, std::string_view charlist = " \t\n\v\f\r");

std::string StrFromWide(std::wstring_view wstr);
std::wstring StrToWide(std::string_view str);

bool IsDigit(char c);
bool IsHexDigit(char c);
bool StrIsDecInteger(std::string_view str);
bool StrIsUnsignedInteger(std::string_view str);
bool StrIsHexNumber(std::string_view str);
bool StrIsBinNumber(std::string_view str);
// Check whether this string is a valid numeric string (a base 10 real number).
bool StrIsNumeric(std::string_view str);

bool StrToBool(std::string_view str);

std::vector<std::string> StrSplit(std::string_view str, std::string_view delimiters,
                                  bool skipEmptySubStr = true);
std::vector<std::string_view> StrSplitViews(std::string_view str, std::string_view delimiters,
                                            bool skipEmptySubStr = true);

bool StrReplaceFirst(std::string& str, std::string_view target, std::string_view rep);
bool StrReplaceLast(std::string& str, std::string_view target, std::string_view rep);
// Returns the replaced count.
// https://github.com/pytorch/pytorch/blob/main/c10/util/StringUtil.cpp
size_t StrReplaceAll(std::string& s, std::string_view from, std::string_view to);

inline bool Contains(std::string_view str, std::string_view sub)
{
    return (str.find(sub) != std::string::npos);
}

inline bool Contains(std::string_view str, const char ch)
{
    return (str.find(ch) != std::string::npos);
}

inline void StrReverse(std::string& str)
{
    std::reverse(str.begin(), str.end());
}

std::vector<std::string> RegexSplit(const std::string& str, const std::regex& expr);

template <std::ranges::input_range R>
std::string RangeToString(R&& list,
                          std::format_string<std::ranges::range_reference_t<R>> elemFormat = "{}",
                          std::string_view sep = ", ")
{
    if (std::ranges::empty(list))
    {
        return {};
    }

    std::string str;
    if constexpr (std::ranges::sized_range<R>)
    {
        // Rough estimate: average element size (assuming 8 chars) + separator
        str.reserve(std::ranges::size(list) * (8 + sep.size()));
    }

    auto it = std::ranges::begin(list);
    auto end = std::ranges::end(list);

    std::format_to(std::back_inserter(str), elemFormat, *it);
    ++it;
    for (; it != end; ++it)
    {
        str += sep;
        std::format_to(std::back_inserter(str), elemFormat, *it);
    }
    return str;
}

// ASCII-only
struct StringLess
{
    using is_transparent = void;
    bool operator()(std::string_view left, std::string_view right) const
    {
        return std::ranges::lexicographical_compare(left, right);
    }
};

// Case-insensitive compare op for std::set/map, ASCII-only.
struct StringLessCaseInsensitive
{
    using is_transparent = void;
    bool operator()(std::string_view left, std::string_view right) const
    {
        return std::ranges::lexicographical_compare(
            left, right,
            [](char a, char b)
            {
                return std::tolower(static_cast<unsigned char>(a)) <
                       std::tolower(static_cast<unsigned char>(b));
            });
    }
};

} // namespace rad
