#pragma once

#include <rad/Common/String.h>

#include <boost/locale.hpp>

namespace rad
{

namespace UTFConv
{

std::string ToUTF8Native(std::wstring_view wstr);

template <typename InputString>
std::string ToUTF8(InputString&& str)
{
    return boost::locale::conv::template utf_to_utf<char>(str);
}

inline std::string ToUTF8(std::wstring_view str)
{
    const wchar_t* beg = str.data();
    const wchar_t* end = str.data() + str.size();
    return boost::locale::conv::template utf_to_utf<char>(beg, end);
}

std::wstring ToWideNative(std::string_view str);

template <typename InputString>
std::wstring ToWide(InputString&& str)
{
    return boost::locale::conv::template utf_to_utf<wchar_t>(str);
}

inline std::wstring ToWide(cstring_view str)
{
    const char* beg = str.data();
    const char* end = str.data() + str.size();
    return boost::locale::conv::template utf_to_utf<wchar_t>(beg, end);
}

template <typename InputString>
std::u8string ToUTF8Char8(InputString&& str)
{
    return boost::locale::conv::template utf_to_utf<char8_t>(str);
}

template <typename InputString>
std::u16string ToUTF16(InputString&& str)
{
    return boost::locale::conv::template utf_to_utf<char16_t>(str);
}

template <typename InputString>
std::u32string ToUTF32(InputString&& str)
{
    return boost::locale::conv::template utf_to_utf<char32_t>(str);
}

} // namespace UTFConv

} // namespace rad
