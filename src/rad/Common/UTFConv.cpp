#include <rad/Common/UTFConv.h>

#if defined(RAD_OS_WINDOWS)
#include <Windows.h>
#endif

#include <cassert>
#include <limits>

namespace rad
{

namespace UTFConv
{

std::string ToUTF8Native(std::wstring_view wstr)
{
#if defined(RAD_OS_WINDOWS)
    assert(wstr.size() <= static_cast<size_t>(std::numeric_limits<int>::max()));
    std::string str;
    const int srcLen = static_cast<int>(wstr.length());
    const DWORD flags = WC_ERR_INVALID_CHARS;
    int charCount =
        ::WideCharToMultiByte(CP_UTF8, flags, wstr.data(), srcLen, nullptr, 0, nullptr, nullptr);
    if (charCount > 0)
    {
        str.resize(charCount, 0);
        ::WideCharToMultiByte(CP_UTF8, flags, wstr.data(), srcLen, str.data(), charCount, nullptr,
                              nullptr);
    }
    return str;
#else
    const wchar_t* beg = wstr.data();
    const wchar_t* end = wstr.data() + wstr.size();
    return boost::locale::conv::utf_to_utf<char>(beg, end);
#endif
}

std::wstring ToWideNative(std::string_view str)
{
#if defined(RAD_OS_WINDOWS)
    assert(str.size() <= static_cast<size_t>(std::numeric_limits<int>::max()));
    std::wstring wstr;
    const int srcLen = static_cast<int>(str.length());
    const DWORD flags = MB_ERR_INVALID_CHARS;
    int charCount = ::MultiByteToWideChar(CP_UTF8, flags, str.data(), srcLen, nullptr, 0);
    if (charCount > 0)
    {
        wstr.resize(charCount, 0);
        ::MultiByteToWideChar(CP_UTF8, flags, str.data(), srcLen, wstr.data(), charCount);
    }
    return wstr;
#else
    const char* beg = str.data();
    const char* end = str.data() + str.size();
    return boost::locale::conv::utf_to_utf<wchar_t>(beg, end);
#endif
}

} // namespace UTFConv

} // namespace rad
