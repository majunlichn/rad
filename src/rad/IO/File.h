#pragma once

#include <rad/Common/String.h>

#include <rad/Common/Flags.h>

#include <cstdint>

#include <chrono>
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
    File(cstring_view path, std::ios_base::openmode mode = std::ios::in);
    ~File();

    // Non-copyable
    File(const File&) = delete;
    File& operator=(const File&) = delete;

    // Movable
    File(File&& other) noexcept;

    File& operator=(File&& other) noexcept;

    bool Open(cstring_view path, std::ios_base::openmode mode = std::ios::in);
    void Close();

    bool IsOpen() const { return m_stream.is_open(); }

    const std::string& GetPath() const { return m_path; }

    uint64_t GetSize() const { return GetSize(m_path); }

    std::fstream& GetStream() { return m_stream; }

    // --- Read / Write Interfaces ---

    // Seeks to a specific position
    bool Seek(int64_t offset, std::ios_base::seekdir dir = std::ios::beg);

    // Returns the current read/write position
    int64_t Tell();

    bool Read(void* data, size_t sizeInBytes);

    bool Write(const void* data, size_t sizeInBytes);

    bool Write(std::string_view text);

    // Reads a single line. Returns true if successful, false if EOF.
    bool ReadLine(std::string& outLine);

    template <typename T>
    File& operator<<(const T& value)
    {
        assert(IsOpen());
        m_stream << value;
        if (!m_stream.good())
        {
            throw std::ios_base::failure("Failed to write to file: " + m_path);
        }
        return *this;
    }

    // Flushes the stream buffer to disk
    void Flush();

    // --- Static Utility Methods ---

    static bool Exists(cstring_view path);

    // File size on disk.
    static uint64_t GetSize(cstring_view fileName);

    // Reads all bytes from a file (binary mode).
    static std::vector<uint8_t> ReadAll(cstring_view path);
    // Reads the entire contents of a file into a string (text mode).
    static std::string ReadAllText(cstring_view path);
    // Reads the entire contents of a file line by line.
    static std::vector<std::string> ReadAllLines(cstring_view path);

    static bool Delete(cstring_view path);

private:
    std::string m_path;
    std::fstream m_stream;

}; // class File

enum class FileType : std::uint32_t
{
    Unknown,         // Unknown file type
    Regular,         // Regular file
    Directory,       // Directory
    Symlink,         // Symbolic link
    CharacterDevice, // Character device
    BlockDevice,     // Block device
    Fifo,            // FIFO / named pipe
    Socket           // Socket
};

// Cross-platform file permission flags (Unix-style, approximated on Windows)
enum class FilePermissionBits : std::uint32_t
{
    None = 0,
    OwnerRead = 0400,
    OwnerWrite = 0200,
    OwnerExecute = 0100,
    GroupRead = 0040,
    GroupWrite = 0020,
    GroupExecute = 0010,
    OthersRead = 0004,
    OthersWrite = 0002,
    OthersExecute = 0001,
    // Common combinations
    OwnerReadWrite = 0600,
    OwnerAll = 0700,
    GroupAll = 0070,
    OthersAll = 0007,
    AllReadWrite = 0666,
    AllExecute = 0111,
    AllAccess = 0777
};

RAD_FLAG_TRAITS(FilePermissionBits, FilePermissionBits::AllAccess);
using FilePermissionFlags = Flags<FilePermissionBits>;

struct FileInfo
{
    FileType fileType;               // File type
    FilePermissionFlags permissions; // Permission bits (Unix-style 0777 format)
    std::uint64_t fileSize;          // File size in bytes
    std::chrono::system_clock::time_point modifyTime;
    std::chrono::system_clock::time_point statusChangeTime;
};

// Convert FileType to string
const char* FileTypeToString(FileType type);

// Convert permission bits to Unix-style string (e.g., "rwxr-xr-x")
std::string FilePermissionsToString(FilePermissionFlags permissions);

// Cross-platform, wraps fstat64.
bool GetFileInfo(cstring_view filePath, FileInfo& info);

} // namespace rad
