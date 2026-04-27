#include <rad/Common/UTFConv.h>

#if defined(RAD_OS_WINDOWS)
#include <Windows.h>
#endif

namespace rad
{

namespace UTFConv
{

std::string ToUTF8Native(std::wstring_view wstr)
{
#if defined(RAD_OS_WINDOWS)
    std::string str;
    int charCount = ::WideCharToMultiByte(CP_UTF8, 0, wstr.data(), static_cast<int>(wstr.length()),
                                          NULL, 0, NULL, NULL);
    if (charCount > 0)
    {
        str.resize(charCount, 0);
        ::WideCharToMultiByte(CP_UTF8, 0, wstr.data(), static_cast<int>(wstr.length()), &str[0],
                              charCount, NULL, NULL);
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
    std::wstring wstr;
    int charCount =
        ::MultiByteToWideChar(CP_UTF8, 0, str.data(), static_cast<int>(str.length()), NULL, 0);
    if (charCount > 0)
    {
        wstr.resize(charCount, 0);
        ::MultiByteToWideChar(CP_UTF8, 0, str.data(), static_cast<int>(str.length()), &wstr[0],
                              charCount);
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
