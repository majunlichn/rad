#pragma once

#include <rad/Common/Platform.h>

#include <pystring/pystring.h>

#include <algorithm>
#include <format>
#include <ranges>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include <regex>
#include <sstream>

#if defined(_WIN32)
#if !defined(strcasecmp)
#define strcasecmp(a, b) _stricmp(a, b)
#endif
#if !defined(strncasecmp)
#define strncasecmp(a, b, n) _strnicmp(a, b, n)
#endif
#endif

namespace rad
{

// std::string is expected to be UTF-8 encoded and null-terminated.

// Non-owning view of a null-terminated string, samiliar to std::string_view
// but ensures null-termination and provides c_str() method.
class cstring_view
{
    const char* m_data;
    size_t m_size;

public:
    using iterator = const char*;
    using const_iterator = const char*;
    using size_type = size_t;

    static constexpr size_t npos = static_cast<size_t>(-1);

    constexpr cstring_view() noexcept
        : m_data(""),
          m_size(0)
    {
    }

    constexpr cstring_view(const char* str) noexcept
        : m_data(str ? str : ""),
          m_size(str ? std::char_traits<char>::length(str) : 0)
    {
    }

    constexpr cstring_view(const char* str, size_t len)
        : m_data(str),
          m_size(len)
    {
        if (str == nullptr)
        {
            m_size = 0;
        }
    }

    cstring_view(const std::string& str) noexcept
        : m_data(str.c_str()),
          m_size(str.length())
    {
    }

    cstring_view(std::string_view) = delete;

    constexpr operator bool() const noexcept
    {
        return m_size > 0;
    }

    constexpr operator std::string_view() const noexcept
    {
        return std::string_view(m_data, m_size);
    }

    constexpr const char* c_str() const noexcept
    {
        return m_data;
    }

    constexpr const char* data() const noexcept
    {
        return m_data;
    }

    constexpr size_t size() const noexcept
    {
        return m_size;
    }

    constexpr size_t length() const noexcept
    {
        return m_size;
    }

    constexpr bool empty() const noexcept
    {
        return m_size == 0;
    }

    constexpr const char& operator[](size_t pos) const noexcept
    {
        return m_data[pos];
    }

    constexpr const char& at(size_t pos) const
    {
        if (pos >= m_size)
        {
            throw std::out_of_range("cstring_view::at");
        }
        return m_data[pos];
    }

    constexpr const char& front() const noexcept
    {
        return m_data[0];
    }

    constexpr const char& back() const noexcept
    {
        return m_data[m_size - 1];
    }

    constexpr const_iterator begin() const noexcept
    {
        return m_data;
    }

    constexpr const_iterator end() const noexcept
    {
        return m_data + m_size;
    }

    constexpr const_iterator cbegin() const noexcept
    {
        return m_data;
    }

    constexpr const_iterator cend() const noexcept
    {
        return m_data + m_size;
    }

}; // class cstring_view

std::string StrPrintf(cstring_view format, ...);
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

std::vector<std::string> StrSplit(std::string_view str, std::string_view delimiters, bool skipEmptySubStr = true);
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

struct StringLess
{
    using is_transparent = void;
    bool operator()(std::string_view left, std::string_view right) const
    {
        return left < right;
    }
};

// Case-insensitive compare op for std::set/map.
struct StringLessCaseInsensitive
{
    using is_transparent = void;
    bool operator()(std::string_view left, std::string_view right) const
    {
        if (left.size() != right.size())
        {
            return left.size() < right.size();
        }
        else
        {
            return strncasecmp(left.data(), right.data(), left.size());
        }
    }
};

template <std::ranges::input_range R>
std::string RangeToString(const R& list, std::string_view elemFormat = "{}", std::string_view sep = ", ")
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

    if (elemFormat.empty() || elemFormat == "{}")
    {
        std::format_to(std::back_inserter(str), "{}", *it);
        ++it;
        for (; it != end; ++it)
        {
            str += sep;
            std::format_to(std::back_inserter(str), "{}", *it);
        }
    }
    else
    {
        std::vformat_to(std::back_inserter(str), elemFormat, std::make_format_args(*it));
        ++it;
        for (; it != end; ++it)
        {
            str += sep;
            std::vformat_to(std::back_inserter(str), elemFormat, std::make_format_args(*it));
        }
    }
    return str;
}

} // namespace rad
