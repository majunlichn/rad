#include <rad/Common/String.h>

#include <cassert>
#include <cctype>
#include <cstdarg>

#include <stdexcept>

#include <boost/algorithm/string.hpp>
#include <boost/locale.hpp>

#if defined(RAD_OS_WINDOWS)
#include <Windows.h>
#endif

namespace rad
{

std::string StrPrintf(cstring_view format, ...)
{
    va_list args;
    va_start(args, format);
    std::string str = StrPrintfV(format, args);
    va_end(args);
    return str;
}

std::string StrPrintfV(cstring_view format, va_list args)
{
    std::string buffer(16, '\0');
    va_list args1;
    va_copy(args1, args);
    const int lengthRequired = vsnprintf(buffer.data(), buffer.size(), format.c_str(), args1);
    va_end(args1);

    if (lengthRequired < 0)
    {
        throw std::runtime_error("format error");
    }

    if (lengthRequired == 0)
    {
        return {};
    }

    if (static_cast<size_t>(lengthRequired) >= buffer.size())
    {
        buffer.resize(static_cast<size_t>(lengthRequired) + 1u, '\0');
        va_list args2;
        va_copy(args2, args);
        const int count = vsnprintf(buffer.data(), buffer.size(), format.c_str(), args2);
        va_end(args2);
        if (count != lengthRequired)
        {
            throw std::runtime_error("format error");
        }
    }

    buffer.resize(static_cast<size_t>(lengthRequired));
    return buffer;
}

bool StrEqual(std::string_view str1, std::string_view str2)
{
    return (str1 == str2);
}

bool StrCaseEqual(std::string_view str1, std::string_view str2)
{
    if (str1.size() != str2.size())
    {
        return false;
    }
#if defined(RAD_COMPILER_MSVC)
    return (_strnicmp(str1.data(), str2.data(), str1.size()) == 0);
#else
    return (strncasecmp(str1.data(), str2.data(), str1.size()) == 0);
#endif
}

std::string StrUpper(std::string_view s)
{
    std::string buffer(s);
    StrUpperInPlace(buffer);
    return buffer;
}

std::string StrLower(std::string_view s)
{
    std::string buffer(s);
    StrLowerInPlace(buffer);
    return buffer;
}

void StrUpperInPlace(std::string& s)
{
    boost::algorithm::to_upper(s);
}

void StrLowerInPlace(std::string& s)
{
    boost::algorithm::to_lower(s);
}

std::string StrTrim(std::string_view str, std::string_view charlist)
{
    if (str.empty())
    {
        return {};
    }
    size_t beg = str.find_first_not_of(charlist);
    if (beg == std::string_view::npos)
    {
        return {};
    }
    size_t end = str.find_last_not_of(charlist) + 1;
    if (beg < end)
    {
        return std::string(str.substr(beg, end - beg));
    }
    else
    {
        return {};
    }
}

void StrTrimInPlace(std::string& str, std::string_view charlist)
{
    str.erase(str.find_last_not_of(charlist) + 1);
    str.erase(0, str.find_first_not_of(charlist));
}

bool IsDigit(char c)
{
    return (c >= '0' && c <= '9');
}

bool IsHexDigit(char c)
{
    return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
}

bool StrIsDecInteger(std::string_view str)
{
    if (str.empty())
    {
        return false;
    }
    size_t i = 0;
    if (str.size() >= 2)
    {
        if (str[0] == '+' || str[0] == '-')
        {
            i++;
        }
    }
    for (; i < str.size(); ++i)
    {
        if (!IsDigit(str[i]))
        {
            return false;
        }
    }
    return true;
}

bool StrIsUnsignedInteger(std::string_view str)
{
    if (str.empty())
    {
        return false;
    }
    if (StrIsHexNumber(str))
    {
        return true;
    }
    for (size_t i = 0; i < str.size(); ++i)
    {
        if (!IsDigit(str[i]))
        {
            return false;
        }
    }
    return true;
}

bool StrIsHexNumber(std::string_view str)
{
    if (str.starts_with("0x") || str.starts_with("0X"))
    {
        for (size_t i = 2; i < str.size(); ++i)
        {
            if (!IsHexDigit(str[i]))
            {
                return false;
            }
        }
        return true;
    }
    return false;
}

bool StrIsBinNumber(std::string_view str)
{
    if (str.starts_with("0b") || str.starts_with("0B"))
    {
        return std::all_of(str.begin() + 2, str.end(), [](char c) { return c == '0' || c == '1'; });
    }
    return false;
}

bool StrIsNumeric(std::string_view str)
{
    const char* p = str.data();
    if ((*p == '-') || (*p == '+'))
    {
        ++p;
    }

    bool hasDot = false;
    while (*p != '\0')
    {
        if (*p == '.')
        {
            if (hasDot)
            {
                return false;
            }
            hasDot = true;
        }
        else if (!IsDigit(*p))
        {
            return false;
        }

        ++p;
    }

    return true;
}

bool StrToBool(std::string_view str)
{
    if (rad::StrEqual(str, "1") || rad::StrCaseEqual(str, "true"))
    {
        return true;
    }
    else
    {
        return false;
    }
}

std::vector<std::string> StrSplit(std::string_view str, std::string_view delimiters,
                                  bool skipEmptySubStr)
{
    std::vector<std::string> tokens;

    std::string::size_type pos = 0;
    std::string::size_type offset = 0;

    while (offset < str.length() + 1)
    {
        pos = str.find_first_of(delimiters, offset);
        if (pos == std::string::npos)
        {
            pos = str.length();
        }
        if (pos != offset || !skipEmptySubStr)
        {
            tokens.push_back(std::string(str.data() + offset, pos - offset));
        }
        offset = pos + 1;
    }

    return tokens;
}

std::vector<std::string_view> StrSplitViews(std::string_view str, std::string_view delimiters,
                                            bool skipEmptySubStr)
{
    std::vector<std::string_view> tokens;

    std::string_view::size_type pos = 0;
    std::string_view::size_type offset = 0;

    while (offset < str.length() + 1)
    {
        pos = str.find_first_of(delimiters, offset);
        if (pos == std::string_view::npos)
        {
            pos = str.length();
        }
        if (pos != offset || !skipEmptySubStr)
        {
            tokens.push_back(std::string_view(str.data() + offset, pos - offset));
        }
        offset = pos + 1;
    }

    return tokens;
}

bool StrReplaceFirst(std::string& str, std::string_view target, std::string_view rep)
{
    const size_t startPos = str.find(target);
    if (startPos == std::string::npos)
    {
        return false;
    }
    str.replace(startPos, target.length(), rep);
    return true;
}

bool StrReplaceLast(std::string& str, std::string_view target, std::string_view rep)
{
    size_t startPos = str.rfind(target);
    if (startPos == std::string::npos)
    {
        return false;
    }
    str.replace(startPos, target.length(), rep);
    return true;
}

size_t StrReplaceAll(std::string& s, std::string_view from, std::string_view to)
{
    if (from.empty())
    {
        return 0;
    }

    size_t numReplaced = 0;
    std::string::size_type last_pos = 0u;
    std::string::size_type cur_pos = 0u;
    std::string::size_type write_pos = 0u;
    const std::string_view input(s);

    if (from.size() >= to.size())
    {
        // If the replacement string is not larger than the original, we
        // can do the replacement in-place without allocating new storage.
        char* s_data = &s[0];

        while ((cur_pos = s.find(from.data(), last_pos, from.size())) != std::string::npos)
        {
            ++numReplaced;
            // Append input between replaced sub-strings
            if (write_pos != last_pos)
            {
                std::copy(s_data + last_pos, s_data + cur_pos, s_data + write_pos);
            }
            write_pos += cur_pos - last_pos;
            // Append the replacement sub-string
            std::copy(to.begin(), to.end(), s_data + write_pos);
            write_pos += to.size();
            // Start search from next character after `from`
            last_pos = cur_pos + from.size();
        }

        // Append any remaining input after replaced sub-strings
        if (write_pos != last_pos)
        {
            std::copy(s_data + last_pos, s_data + input.size(), s_data + write_pos);
            write_pos += input.size() - last_pos;
            s.resize(write_pos);
        }
        return numReplaced;
    }

    // Otherwise, do an out-of-place replacement in a temporary buffer
    std::string buffer;

    while ((cur_pos = s.find(from.data(), last_pos, from.size())) != std::string::npos)
    {
        ++numReplaced;
        // Append input between replaced sub-strings
        buffer.append(input.begin() + last_pos, input.begin() + cur_pos);
        // Append the replacement sub-string
        buffer.append(to.begin(), to.end());
        // Start search from next character after `from`
        last_pos = cur_pos + from.size();
    }
    if (numReplaced == 0)
    {
        // If nothing was replaced, don't modify the input
        return 0;
    }
    // Append any remaining input after replaced sub-strings
    buffer.append(input.begin() + last_pos, input.end());
    s = std::move(buffer);
    return numReplaced;
}

std::vector<std::string> RegexSplit(const std::string& str, const std::regex& expr)
{
    std::vector<std::string> tokens;
    std::sregex_token_iterator iter(str.begin(), str.end(), expr, -1);
    std::sregex_token_iterator end;
    while (iter != end)
    {
        tokens.push_back(*iter);
        ++iter;
    }
    return tokens;
}

} // namespace rad
