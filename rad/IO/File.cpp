#include <rad/IO/File.h>

#include <rad/Common/Integer.h>

namespace rad
{

std::vector<uint8_t> File::ReadAll(cstring_view path)
{
    File file;
    if (!file.Open(path, std::ios::in | std::ios::binary))
    {
        return {};
    }

    size_t size = file.GetSize();
    if (size == 0)
    {
        return {};
    }

    std::vector<uint8_t> buffer(size);
    file.Read(buffer.data(), size);
    buffer.resize(file.GetStream().gcount());
    return buffer;
}

std::string File::ReadAllText(cstring_view path)
{
    File file;
    if (!file.Open(path, std::ios::in))
    {
        return {};
    }

    size_t size = file.GetSize();
    if (size == 0)
    {
        return {};
    }

    std::string content;
    content.resize(size);
    file.Read(content.data(), size);
    // On Windows, CRLF is converted to LF, meaning fewer bytes are read.
    // gcount() gives the exact number of characters extracted.
    content.resize(file.GetStream().gcount());
    return content;
}

std::vector<std::string> File::ReadAllLines(cstring_view path)
{
    File file;
    if (!file.Open(path, std::ios::in))
    {
        return {};
    }

    std::vector<std::string> lines;

    // Estimate line count to avoid frequent re-allocations (assume ~40 chars per line on average)
    size_t size = file.GetSize();
    if (size > 0)
    {
        lines.reserve(DivRoundUp<size_t>(size, 40));
    }

    std::string line;
    while (file.ReadLine(line))
    {
        lines.push_back(std::move(line));
    }
    return lines;
}

} // namespace rad
