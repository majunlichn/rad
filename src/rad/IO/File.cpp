#ifndef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 64
#endif
#include <rad/IO/File.h>

#include <rad/Common/Integer.h>

#if defined(RAD_OS_WINDOWS)
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <fcntl.h>
#else
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#endif

namespace rad
{

File::File(cstring_view path, std::ios_base::openmode mode)
{
    Open(path, mode);
}

File::~File()
{
    Close();
}

File::File(File&& other) noexcept :
    m_stream(std::move(other.m_stream)),
    m_path(std::move(other.m_path))
{
}

File& File::operator=(File&& other) noexcept
{
    if (this != &other)
    {
        Close();
        m_stream = std::move(other.m_stream);
        m_path = std::move(other.m_path);
    }
    return *this;
}

bool File::Open(cstring_view path, std::ios_base::openmode mode)
{
    Close();
    m_path = path;
    m_stream.open(FilePath((const char8_t*)m_path.c_str()), mode);
    return m_stream.is_open();
}

void File::Close()
{
    if (m_stream.is_open())
    {
        m_stream.close();
        m_path.clear();
    }
}

bool File::Seek(int64_t offset, std::ios_base::seekdir dir)
{
    if (!IsOpen())
    {
        return false;
    }
    // The underlying std::filebuf maintains only one file position.
    m_stream.seekg(offset, dir);
    return m_stream.good();
}

int64_t File::Tell()
{
    // The underlying std::filebuf maintains only one file position.
    return IsOpen() ? static_cast<int64_t>(m_stream.tellg()) : 0;
}

bool File::Read(void* data, size_t sizeInBytes)
{
    if (!IsOpen())
    {
        return false;
    }
    m_stream.read(reinterpret_cast<char*>(data), sizeInBytes);
    return m_stream.good() || m_stream.eof();
}

bool File::Write(const void* data, size_t sizeInBytes)
{
    if (!IsOpen())
    {
        return false;
    }
    m_stream.write(reinterpret_cast<const char*>(data), sizeInBytes);
    return m_stream.good();
}

bool File::Write(std::string_view text)
{
    return Write(text.data(), text.size());
}

bool File::ReadLine(std::string& outLine)
{
    if (!IsOpen())
    {
        return false;
    }
    return static_cast<bool>(std::getline(m_stream, outLine));
}

void File::Flush()
{
    if (IsOpen())
    {
        m_stream.flush();
    }
}

bool File::Exists(cstring_view path)
{
    std::error_code ec;
    bool result = std::filesystem::exists((const char8_t*)path.c_str(), ec);
    return !ec && result;
}

uint64_t File::GetSize(cstring_view fileName)
{
    std::error_code ec;
    auto size = std::filesystem::file_size((const char8_t*)fileName.c_str(), ec);
    return !ec ? static_cast<uint64_t>(size) : 0;
}

std::vector<uint8_t> File::ReadAll(cstring_view path)
{
    File file;
    if (!file.Open(path, std::ios::in | std::ios::binary))
    {
        return {};
    }

    uint64_t size = file.GetSize();
    if (size == 0)
    {
        return {};
    }

    if constexpr ((sizeof(size_t) == 4) && (size > UINT32_MAX))
    {
        // File is too large to read into the buffer on a 32-bit system.
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

bool File::Delete(cstring_view path)
{
    std::error_code ec;
    bool result = std::filesystem::remove((const char8_t*)path.c_str(), ec);
    return !ec && result;
}

// Convert platform-specific mode to cross-platform FileType
static FileType GetFileTypeFromMode(std::uint32_t mode)
{
#if defined(_WIN32)
    switch (mode & _S_IFMT)
    {
    case _S_IFREG:
        return FileType::Regular;
    case _S_IFDIR:
        return FileType::Directory;
    case _S_IFCHR:
        return FileType::CharacterDevice;
    case _S_IFIFO:
        return FileType::Fifo;
    default:
        return FileType::Unknown;
    }
#else
    if (S_ISREG(mode))
        return FileType::Regular;
    if (S_ISDIR(mode))
        return FileType::Directory;
    if (S_ISLNK(mode))
        return FileType::Symlink;
    if (S_ISCHR(mode))
        return FileType::CharacterDevice;
    if (S_ISBLK(mode))
        return FileType::BlockDevice;
    if (S_ISFIFO(mode))
        return FileType::Fifo;
    if (S_ISSOCK(mode))
        return FileType::Socket;
    return FileType::Unknown;
#endif
}

const char* FileTypeToString(FileType type)
{
    switch (type)
    {
    case FileType::Regular:
        return "Regular File";
    case FileType::Directory:
        return "Directory";
    case FileType::Symlink:
        return "Symbolic Link";
    case FileType::CharacterDevice:
        return "Character Device";
    case FileType::BlockDevice:
        return "Block Device";
    case FileType::Fifo:
        return "FIFO Pipe";
    case FileType::Socket:
        return "Socket";
    default:
        return "Unknown";
    }
}

std::string FilePermissionsToString(FilePermissionFlags permissions)
{
    std::string str(9, '\0');
    auto FormatFilePerms = [](char* str, FilePermissionFlags bits, FilePermissionFlags readBit,
                              FilePermissionFlags writeBit, FilePermissionFlags execBit)
    {
        str[0] = (bits & readBit) ? 'r' : '-';
        str[1] = (bits & writeBit) ? 'w' : '-';
        str[2] = (bits & execBit) ? 'x' : '-';
    };
    FormatFilePerms(str.data() + 0, permissions, FilePermissionBits::OwnerRead,
                    FilePermissionBits::OwnerWrite,
                    FilePermissionBits::OwnerExecute); // Owner
    FormatFilePerms(str.data() + 3, permissions, FilePermissionBits::GroupRead,
                    FilePermissionBits::GroupWrite,
                    FilePermissionBits::GroupExecute); // Group
    FormatFilePerms(str.data() + 6, permissions, FilePermissionBits::OthersRead,
                    FilePermissionBits::OthersWrite,
                    FilePermissionBits::OthersExecute); // Others
    return str;
}

#if defined(RAD_OS_WINDOWS)

bool GetFileInfo(cstring_view filePath, FileInfo& info)
{
    int fd = _open(filePath.c_str(), _O_RDONLY | _O_BINARY);
    if (fd == -1)
    {
        return false;
    }

    struct _stat64 st;
    int result = _fstat64(fd, &st);
    _close(fd);

    if (result != 0)
    {
        return false;
    }

    info.fileType = GetFileTypeFromMode(static_cast<std::uint32_t>(st.st_mode));
    info.permissions = FilePermissionFlags::FromMask(st.st_mode & 0777);
    info.fileSize = static_cast<std::uint64_t>(st.st_size);

    // Convert __time64_t (seconds since epoch) to system_clock::time_point
    auto MakeTimePoint = [](__time64_t time) -> std::chrono::system_clock::time_point
    {
        if (time != 0)
        {
            return std::chrono::system_clock::time_point(std::chrono::seconds(time));
        }
        return std::chrono::system_clock::time_point{};
    };

    info.modifyTime = MakeTimePoint(st.st_mtime);
    info.statusChangeTime = MakeTimePoint(st.st_ctime);

    return true;
}

#else // POSIX Implementation

bool GetFileInfo(cstring_view filePath, FileInfo& info)
{
    struct stat st;
    // Use lstat to get info about the symlink itself, not its target.
    if (lstat(filePath.c_str(), &st) != 0)
    {
        return false;
    }

    info.fileType = GetFileTypeFromMode(static_cast<std::uint32_t>(st.st_mode));
    info.permissions = FilePermissionFlags::FromMask(st.st_mode & 0777);
    assert(st.st_size >= 0);
    info.fileSize = st.st_size > 0 ? static_cast<std::uint64_t>(st.st_size) : 0;

    // Convert timespec (seconds + nanoseconds) to system_clock::time_point
    auto MakeTimePoint = [](const struct timespec& ts) -> std::chrono::system_clock::time_point
    {
        using namespace std::chrono;
        auto sinceEpoch =
            duration_cast<system_clock::duration>(seconds(ts.tv_sec) + nanoseconds(ts.tv_nsec));
        return system_clock::time_point(sinceEpoch);
    };

#if defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
    // macOS and BSD use st_*timespec naming convention
    info.modifyTime = MakeTimePoint(st.st_mtimespec);
    info.statusChangeTime = MakeTimePoint(st.st_ctimespec);
#else
    // Linux uses st_*tim naming convention (POSIX.1-2008)
    info.modifyTime = MakeTimePoint(st.st_mtim);
    info.statusChangeTime = MakeTimePoint(st.st_ctim);
#endif

    return true;
}

#endif

} // namespace rad
