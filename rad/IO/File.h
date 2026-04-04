#pragma once

#include <rad/Common/String.h>

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <system_error>
#include <vector>

namespace rad
{

using FilePath = std::filesystem::path;

class File
{
public:
    File() = default;

    File(cstring_view path, std::ios_base::openmode mode = std::ios::in)
    {
        Open(path, mode);
    }

    ~File()
    {
        Close();
    }

    // Non-copyable
    File(const File&) = delete;
    File& operator=(const File&) = delete;

    // Movable
    File(File&& other) noexcept
        : m_stream(std::move(other.m_stream)),
          m_path(std::move(other.m_path))
    {
    }

    File& operator=(File&& other) noexcept
    {
        if (this != &other)
        {
            Close();
            m_stream = std::move(other.m_stream);
            m_path = std::move(other.m_path);
        }
        return *this;
    }

    bool Open(cstring_view path, std::ios_base::openmode mode = std::ios::in)
    {
        Close();
        m_path = path;
        m_stream.open(reinterpret_cast<const char8_t*>(m_path.data()), mode);
        return m_stream.is_open();
    }

    void Close()
    {
        if (m_stream.is_open())
        {
            m_stream.close();
        }
    }

    bool IsOpen() const
    {
        return m_stream.is_open();
    }

    const std::string& GetPath() const
    {
        return m_path;
    }

    uint64_t GetSize() const
    {
        return GetSize(m_path);
    }

    std::fstream& GetStream()
    {
        return m_stream;
    }

    // --- Read / Write Interfaces ---

    bool Read(void* data, size_t sizeInBytes)
    {
        if (!IsOpen())
        {
            return false;
        }
        m_stream.read(reinterpret_cast<char*>(data), sizeInBytes);
        return m_stream.good() || m_stream.eof();
    }

    bool Write(const void* data, size_t sizeInBytes)
    {
        if (!IsOpen())
        {
            return false;
        }
        m_stream.write(reinterpret_cast<const char*>(data), sizeInBytes);
        return m_stream.good();
    }

    bool Write(std::string_view text)
    {
        return Write(text.data(), text.size());
    }

    // Output a single line. Returns true if successful, false if EOF.
    bool ReadLine(std::string& outLine)
    {
        if (!IsOpen())
        {
            return false;
        }
        return static_cast<bool>(std::getline(m_stream, outLine));
    }

    // --- Static Utility Methods ---
    // File size on disk.
    static uint64_t GetSize(cstring_view fileName)
    {
        std::error_code ec;
        auto size = std::filesystem::file_size((const char8_t*)fileName.data(), ec);
        return ec ? 0 : static_cast<uint64_t>(size);
    }
    // Reads all bytes from a file (binary mode).
    static std::vector<uint8_t> ReadAll(cstring_view path);
    // Reads the entire contents of a file into a string (text mode).
    static std::string ReadAllText(cstring_view path);
    // Reads the entire contents of a file line by line.
    static std::vector<std::string> ReadAllLines(cstring_view path);

private:
    std::string m_path;
    std::fstream m_stream;

}; // class File

} // namespace rad
