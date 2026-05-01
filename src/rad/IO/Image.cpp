#include <rad/Common/MemoryDebug.h>

#if defined(_MSC_VER)
#define _CRT_SECURE_NO_WARNINGS 1
#endif
#include <rad/IO/Image.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#define STBI_MALLOC(sz) malloc(sz)
#define STBI_REALLOC(p, newsz) realloc(p, newsz)
#define STBI_FREE(p) free(p)
#if defined(RAD_OS_WINDOWS)
#define STBI_WINDOWS_UTF8
#endif
#include <stb_image.h>
#include <stb_image_resize2.h>
#include <stb_image_write.h>

#include <algorithm>
#include <cassert>
#include <cstring>
#include <limits>
#include <optional>

// Helpers for stb_image / stb_image_write usage in this TU only (not the stbi_* C API symbols).
namespace stbi
{

/// Channel counts supported by our STB load/write/resize paths (stbi layout / writers).
constexpr bool CheckChannelCount(int channelCount) noexcept
{
    return channelCount >= 1 && channelCount <= 4;
}

constexpr bool CheckJpegQuality(int quality) noexcept
{
    return quality >= 1 && quality <= 100;
}

/// \p desiredChannels for stbi_load: 0 = keep source component count, else must be CheckChannelCount.
constexpr bool CheckDesiredChannelCount(int desiredChannels) noexcept
{
    return desiredChannels == 0 || CheckChannelCount(desiredChannels);
}

/// Packed row size in bytes, or -1 if multiply overflows int / size_t.
int PackedRowStrideBytes(int width, int channelCount) noexcept
{
    const std::size_t w = static_cast<std::size_t>(width);
    const std::size_t c = static_cast<std::size_t>(channelCount);
    const std::size_t row = w * c;
    if (c != 0 && row / c != w)
    {
        return -1;
    }
    if (row > static_cast<std::size_t>(std::numeric_limits<int>::max()))
    {
        return -1;
    }
    return static_cast<int>(row);
}

/// Packed image size in bytes, or nullopt if dimensions are invalid or the product overflows.
[[nodiscard]] std::optional<std::size_t> GetPackedImageByteCount(int width, int height,
                                                                 int channelCount) noexcept
{
    if (width <= 0 || height <= 0 || !CheckChannelCount(channelCount))
    {
        return std::nullopt;
    }
    const std::size_t w = static_cast<std::size_t>(width);
    const std::size_t h = static_cast<std::size_t>(height);
    const std::size_t c = static_cast<std::size_t>(channelCount);
    const std::size_t rowBytes = w * c;
    if (c != 0 && rowBytes / c != w)
    {
        return std::nullopt;
    }
    const std::size_t total = rowBytes * h;
    if (rowBytes != 0 && total / rowBytes != h)
    {
        return std::nullopt;
    }
    return total;
}

/// Byte offset of pixel (left, top) in a packed row-major buffer, or nullopt if invalid / overflow.
[[nodiscard]] std::optional<std::size_t> GetSubRectByteOffset(int imgWidth, int imgHeight,
                                                              int channelCount, int left,
                                                              int top) noexcept
{
    if (!CheckChannelCount(channelCount) || imgWidth <= 0 || imgHeight <= 0)
    {
        return std::nullopt;
    }
    if (left < 0 || top < 0 || left >= imgWidth || top >= imgHeight)
    {
        return std::nullopt;
    }
    const std::size_t mw = static_cast<std::size_t>(imgWidth);
    const std::size_t c = static_cast<std::size_t>(channelCount);
    const std::size_t rowPixels = static_cast<std::size_t>(top) * mw;
    if (top != 0 && mw != 0 && rowPixels / mw != static_cast<std::size_t>(top))
    {
        return std::nullopt;
    }
    const std::size_t pixIndex = rowPixels + static_cast<std::size_t>(left);
    if (pixIndex < rowPixels)
    {
        return std::nullopt;
    }
    const std::size_t offsetBytes = pixIndex * c;
    if (pixIndex != 0 && offsetBytes / pixIndex != c)
    {
        return std::nullopt;
    }
    return offsetBytes;
}

/// Exclusive end of the byte range stbi_write_png reads for this crop, or nullopt if out of bounds.
[[nodiscard]] std::optional<std::size_t> GetPngSubRectReadEndExclusive(
    std::size_t totalBytes, std::size_t offsetBytes, int subWidth, int subHeight, int channelCount,
    int parentStrideBytes) noexcept
{
    if (subWidth <= 0 || subHeight <= 0 || !CheckChannelCount(channelCount))
    {
        return std::nullopt;
    }
    const int packedSub = PackedRowStrideBytes(subWidth, channelCount);
    if (packedSub < 0 || parentStrideBytes < packedSub)
    {
        return std::nullopt;
    }
    const std::size_t sw = static_cast<std::size_t>(subWidth);
    const std::size_t sh = static_cast<std::size_t>(subHeight);
    const std::size_t c = static_cast<std::size_t>(channelCount);
    const std::size_t stride = static_cast<std::size_t>(parentStrideBytes);
    const std::size_t subRowPacked = sw * c;
    if (c != 0 && subRowPacked / c != sw)
    {
        return std::nullopt;
    }
    const std::size_t rowSkip = (sh - 1U) * stride;
    if (sh > 1U && stride != 0U && rowSkip / stride != (sh - 1U))
    {
        return std::nullopt;
    }
    const std::size_t lastRowStart = offsetBytes + rowSkip;
    if (lastRowStart < offsetBytes)
    {
        return std::nullopt;
    }
    const std::size_t endExclusive = lastRowStart + subRowPacked;
    if (endExclusive < lastRowStart || endExclusive > totalBytes)
    {
        return std::nullopt;
    }
    return endExclusive;
}

} // namespace stbi

namespace rad
{

void ImageUnorm8::SetHdrToLdrGamma(float gamma)
{
    stbi_hdr_to_ldr_gamma(gamma);
}

void ImageUnorm8::SetHdrToLdrScale(float scale)
{
    stbi_hdr_to_ldr_scale(scale);
}

ImageUnorm8::ImageUnorm8()
{
}

ImageUnorm8::~ImageUnorm8()
{
    Destroy();
}

void ImageUnorm8::Destroy()
{
    if (m_data)
    {
        free(m_data);
        m_data = nullptr;
    }
    m_width = 0;
    m_height = 0;
    m_channelCount = 0;
}

bool ImageUnorm8::Allocate(int width, int height, int channelCount, int clearValue)
{
    Destroy();
    assert(m_data == nullptr);
    if (width <= 0 || height <= 0 || !stbi::CheckChannelCount(channelCount))
    {
        return false;
    }
    const size_t w = static_cast<size_t>(width);
    const size_t h = static_cast<size_t>(height);
    const size_t c = static_cast<size_t>(channelCount);
    const size_t rowBytes = w * c;
    if (c != 0 && rowBytes / c != w)
    {
        return false;
    }
    const size_t dataSize = rowBytes * h;
    if (rowBytes != 0 && dataSize / rowBytes != h)
    {
        return false;
    }
    if (clearValue < 0 || clearValue > std::numeric_limits<unsigned char>::max())
    {
        return false;
    }
    m_data = reinterpret_cast<uint8_t*>(malloc(dataSize));
    if (m_data)
    {
        std::memset(m_data, clearValue, dataSize);
        m_width = width;
        m_height = height;
        m_channelCount = channelCount;
        return true;
    }
    return false;
}

bool ImageUnorm8::Fill(uint8_t value)
{
    if (!m_data || m_width <= 0 || m_height <= 0 || !stbi::CheckChannelCount(m_channelCount))
    {
        return false;
    }
    const size_t dataSize = GetDataSize();
    std::memset(m_data, static_cast<int>(value), dataSize);
    return true;
}

bool ImageUnorm8::GetFileInfo(cstring_view fileName, int* width, int* height, int* channelCount)
{
    int ok = stbi_info(fileName.c_str(), width, height, channelCount);
    return (ok != 0);
}

bool ImageUnorm8::LoadFromFile(cstring_view fileName, int channelCount)
{
    Destroy();
    assert(m_data == nullptr);
    if (!stbi::CheckDesiredChannelCount(channelCount))
    {
        return false;
    }

    int originalChannelCount = 0;
    uint8_t* loaded = (uint8_t*)stbi_load(fileName.c_str(), &m_width, &m_height,
                                          &originalChannelCount, channelCount);
    m_data = loaded;
    if (m_data)
    {
        m_channelCount = (channelCount > 0) ? channelCount : originalChannelCount;
        if (!stbi::CheckChannelCount(m_channelCount))
        {
            free(m_data);
            m_data = nullptr;
            m_width = 0;
            m_height = 0;
            m_channelCount = 0;
            return false;
        }
        return true;
    }
    m_width = 0;
    m_height = 0;
    m_channelCount = 0;
    return false;
}

bool ImageUnorm8::LoadFromMemory(const void* buffer, size_t bufferSize, int channelCount)
{
    Destroy();
    assert(m_data == nullptr);
    if (!buffer && bufferSize != 0)
    {
        return false;
    }
    if (!stbi::CheckDesiredChannelCount(channelCount))
    {
        return false;
    }

    int originalChannelCount = 0;
    uint8_t* loaded =
        (uint8_t*)stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(buffer), bufferSize,
                                        &m_width, &m_height, &originalChannelCount, channelCount);
    m_data = loaded;
    if (m_data)
    {
        m_channelCount = (channelCount > 0) ? channelCount : originalChannelCount;
        if (!stbi::CheckChannelCount(m_channelCount))
        {
            free(m_data);
            m_data = nullptr;
            m_width = 0;
            m_height = 0;
            m_channelCount = 0;
            return false;
        }
        return true;
    }
    m_width = 0;
    m_height = 0;
    m_channelCount = 0;
    return false;
}

Ref<ImageUnorm8> ImageUnorm8::Create(int width, int height, int channelCount, int clearValue)
{
    Ref<ImageUnorm8> newImage = new ImageUnorm8();
    if (newImage->Allocate(width, height, channelCount, clearValue))
    {
        return newImage;
    }
    else
    {
        return nullptr;
    }
}

Ref<ImageUnorm8> ImageUnorm8::CreateFromFile(cstring_view fileName, int channelCount)
{
    Ref<ImageUnorm8> newImage = new ImageUnorm8();
    if (newImage->LoadFromFile(fileName, channelCount))
    {
        return newImage;
    }
    else
    {
        return nullptr;
    }
}

Ref<ImageUnorm8> ImageUnorm8::CreateFromMemory(const void* buffer, size_t bufferSize,
                                               int channelCount)
{
    Ref<ImageUnorm8> newImage = new ImageUnorm8();
    if (newImage->LoadFromMemory(buffer, bufferSize, channelCount))
    {
        return newImage;
    }
    else
    {
        return nullptr;
    }
}

Ref<ImageUnorm8> ImageUnorm8::Resize(int newWidth, int newHeight) const
{
    if (!m_data || newWidth <= 0 || newHeight <= 0 || !stbi::CheckChannelCount(m_channelCount))
    {
        return nullptr;
    }

    Ref<ImageUnorm8> newImage = new ImageUnorm8();
    const size_t nw = static_cast<size_t>(newWidth);
    const size_t nh = static_cast<size_t>(newHeight);
    const size_t cc = static_cast<size_t>(m_channelCount);
    const size_t rowBytes = nw * cc;
    if (cc != 0 && rowBytes / cc != nw)
    {
        return nullptr;
    }
    const size_t newSize = rowBytes * nh;
    if (rowBytes != 0 && newSize / rowBytes != nh)
    {
        return nullptr;
    }
    newImage->m_data = reinterpret_cast<uint8_t*>(malloc(newSize));
    if (!newImage->m_data) [[unlikely]]
    {
        return nullptr;
    }
    newImage->m_width = newWidth;
    newImage->m_height = newHeight;
    newImage->m_channelCount = m_channelCount;

    stbir_pixel_layout layout = static_cast<stbir_pixel_layout>(m_channelCount);
    void* result = stbir_resize_uint8_linear(m_data, m_width, m_height, 0, newImage->m_data,
                                             newWidth, newHeight, 0, layout);
    if (!result) [[unlikely]]
    {
        return nullptr;
    }
    return newImage;
}

bool ImageUnorm8::WritePNG(cstring_view fileName) const
{
    const int stride = stbi::PackedRowStrideBytes(m_width, m_channelCount);
    if (stride < 0)
    {
        return false;
    }
    return WritePNG(fileName, stride);
}

bool ImageUnorm8::WritePNG(cstring_view fileName, int strideBytesPerRow) const
{
    if (!m_data || m_width <= 0 || m_height <= 0 || !stbi::CheckChannelCount(m_channelCount))
    {
        return false;
    }
    const int packed = stbi::PackedRowStrideBytes(m_width, m_channelCount);
    if (packed < 0 || strideBytesPerRow < packed)
    {
        return false;
    }
    const int result = stbi_write_png(fileName.c_str(), m_width, m_height, m_channelCount, m_data,
                                      strideBytesPerRow);
    return (result != 0); // returns 0 on failure and non-0 on success.
}

bool ImageUnorm8::WritePNG(cstring_view fileName, int left, int top, int right, int bottom) const
{
    // Sub-rectangle in pixel coordinates: [left, right) x [top, bottom) (half-open intervals).
    if (!m_data || m_width <= 0 || m_height <= 0 || !stbi::CheckChannelCount(m_channelCount))
    {
        return false;
    }
    if (left < 0 || top < 0 || right <= left || bottom <= top || right > m_width ||
        bottom > m_height)
    {
        return false;
    }
    const int width = right - left;
    const int height = bottom - top;
    const auto offset = stbi::GetSubRectByteOffset(m_width, m_height, m_channelCount, left, top);
    const auto totalBytes = stbi::GetPackedImageByteCount(m_width, m_height, m_channelCount);
    const int parentStride = stbi::PackedRowStrideBytes(m_width, m_channelCount);
    if (!offset || !totalBytes || parentStride < 0)
    {
        return false;
    }
    if (!stbi::GetPngSubRectReadEndExclusive(*totalBytes, *offset, width, height, m_channelCount,
                                             parentStride))
    {
        return false;
    }
    const int result = stbi_write_png(fileName.c_str(), width, height, m_channelCount,
                                      m_data + *offset, parentStride);
    return (result != 0); // returns 0 on failure and non-0 on success.
}

bool ImageUnorm8::WriteBMP(cstring_view fileName) const
{
    if (!m_data || m_width <= 0 || m_height <= 0 || !stbi::CheckChannelCount(m_channelCount))
    {
        return false;
    }
    const int result = stbi_write_bmp(fileName.c_str(), m_width, m_height, m_channelCount, m_data);
    return (result != 0); // returns 0 on failure and non-0 on success.
}

bool ImageUnorm8::WriteTGA(cstring_view fileName) const
{
    if (!m_data || m_width <= 0 || m_height <= 0 || !stbi::CheckChannelCount(m_channelCount))
    {
        return false;
    }
    const int result = stbi_write_tga(fileName.c_str(), m_width, m_height, m_channelCount, m_data);
    return (result != 0); // returns 0 on failure and non-0 on success.
}

bool ImageUnorm8::WriteJPG(cstring_view fileName, int quality) const
{
    if (!m_data || m_width <= 0 || m_height <= 0 || !stbi::CheckChannelCount(m_channelCount))
    {
        return false;
    }
    if (!stbi::CheckJpegQuality(quality))
    {
        return false;
    }
    const int result =
        stbi_write_jpg(fileName.c_str(), m_width, m_height, m_channelCount, m_data, quality);
    return (result != 0); // returns 0 on failure and non-0 on success.
}

ImageFloat32::ImageFloat32()
{
}

ImageFloat32::~ImageFloat32()
{
    Destroy();
}

void ImageFloat32::Destroy()
{
    if (m_data)
    {
        free(m_data);
        m_data = nullptr;
    }
    m_width = 0;
    m_height = 0;
    m_channelCount = 0;
}

bool ImageFloat32::Allocate(int width, int height, int channelCount, float clearValue)
{
    Destroy();
    assert(m_data == nullptr);
    if (width <= 0 || height <= 0 || !stbi::CheckChannelCount(channelCount))
    {
        return false;
    }
    float* data = nullptr;
    const size_t w = static_cast<size_t>(width);
    const size_t h = static_cast<size_t>(height);
    const size_t c = static_cast<size_t>(channelCount);
    const size_t floatsPerRow = w * c;
    if (c != 0 && floatsPerRow / c != w)
    {
        return false;
    }
    const size_t floatCount = floatsPerRow * h;
    if (floatsPerRow != 0 && floatCount / floatsPerRow != h)
    {
        return false;
    }
    const size_t sizeInBytes = floatCount * sizeof(float);
    data = (float*)malloc(sizeInBytes);

    if (data)
    {
        m_data = data;
        std::fill_n(m_data, floatCount, clearValue);
        m_width = width;
        m_height = height;
        m_channelCount = channelCount;
        return true;
    }
    else
    {
        return false;
    }
}

bool ImageFloat32::Fill(float value)
{
    if (!m_data || m_width <= 0 || m_height <= 0 || !stbi::CheckChannelCount(m_channelCount))
    {
        return false;
    }
    std::fill_n(m_data, GetDataSize(), value);
    return true;
}

bool ImageFloat32::GetFileInfo(cstring_view fileName, int* width, int* height, int* channelCount)
{
    int ok = stbi_info(fileName.c_str(), width, height, channelCount);
    return (ok != 0);
}

void ImageFloat32::SetLdrToHdrGamma(float gamma)
{
    stbi_ldr_to_hdr_gamma(gamma);
}

void ImageFloat32::SetLdrToHdrScale(float scale)
{
    stbi_ldr_to_hdr_scale(scale);
}

bool ImageFloat32::LoadFromFile(cstring_view fileName, int channelCount)
{
    Destroy();
    assert(m_data == nullptr);
    if (!stbi::CheckDesiredChannelCount(channelCount))
    {
        return false;
    }

    int originalChannelCount = 0;
    float* loaded =
        stbi_loadf(fileName.c_str(), &m_width, &m_height, &originalChannelCount, channelCount);
    m_data = loaded;
    if (m_data)
    {
        m_channelCount = (channelCount > 0) ? channelCount : originalChannelCount;
        if (!stbi::CheckChannelCount(m_channelCount))
        {
            free(m_data);
            m_data = nullptr;
            m_width = 0;
            m_height = 0;
            m_channelCount = 0;
            return false;
        }
        return true;
    }
    m_width = 0;
    m_height = 0;
    m_channelCount = 0;
    return false;
}

bool ImageFloat32::LoadFromMemory(const void* buffer, size_t bufferSize, int channelCount)
{
    Destroy();
    assert(m_data == nullptr);
    if (!buffer && bufferSize != 0)
    {
        return false;
    }
    if (!stbi::CheckDesiredChannelCount(channelCount))
    {
        return false;
    }

    int originalChannelCount = 0;
    float* loaded =
        stbi_loadf_from_memory(reinterpret_cast<const stbi_uc*>(buffer), bufferSize, &m_width,
                               &m_height, &originalChannelCount, channelCount);
    m_data = loaded;
    if (m_data)
    {
        m_channelCount = (channelCount > 0) ? channelCount : originalChannelCount;
        if (!stbi::CheckChannelCount(m_channelCount))
        {
            free(m_data);
            m_data = nullptr;
            m_width = 0;
            m_height = 0;
            m_channelCount = 0;
            return false;
        }
        return true;
    }
    m_width = 0;
    m_height = 0;
    m_channelCount = 0;
    return false;
}

Ref<ImageFloat32> ImageFloat32::Create(int width, int height, int channelCount, float clearValue)
{
    Ref<ImageFloat32> image = new ImageFloat32();
    if (image->Allocate(width, height, channelCount, clearValue))
    {
        return image;
    }
    else
    {
        return nullptr;
    }
}

Ref<ImageFloat32> ImageFloat32::CreateFromFile(cstring_view fileName, int channelCount)
{
    Ref<ImageFloat32> image = new ImageFloat32();
    if (image->LoadFromFile(fileName, channelCount))
    {
        return image;
    }
    else
    {
        return nullptr;
    }
}

Ref<ImageFloat32> ImageFloat32::CreateFromMemory(const void* buffer, size_t bufferSize,
                                                 int channelCount)
{
    Ref<ImageFloat32> newImage = new ImageFloat32();
    if (newImage->LoadFromMemory(buffer, bufferSize, channelCount))
    {
        return newImage;
    }
    else
    {
        return nullptr;
    }
}

Ref<ImageFloat32> ImageFloat32::Resize(int newWidth, int newHeight) const
{
    if (!m_data || newWidth <= 0 || newHeight <= 0 || !stbi::CheckChannelCount(m_channelCount))
    {
        return nullptr;
    }

    Ref<ImageFloat32> newImage = new ImageFloat32();
    const size_t nw = static_cast<size_t>(newWidth);
    const size_t nh = static_cast<size_t>(newHeight);
    const size_t cc = static_cast<size_t>(m_channelCount);
    const size_t floatsPerRow = nw * cc;
    if (cc != 0 && floatsPerRow / cc != nw)
    {
        return nullptr;
    }
    const size_t floatCount = floatsPerRow * nh;
    if (floatsPerRow != 0 && floatCount / floatsPerRow != nh)
    {
        return nullptr;
    }
    const size_t newSizeInBytes = floatCount * sizeof(float);
    newImage->m_data = reinterpret_cast<float*>(malloc(newSizeInBytes));
    if (!newImage->m_data) [[unlikely]]
    {
        return nullptr;
    }
    newImage->m_width = newWidth;
    newImage->m_height = newHeight;
    newImage->m_channelCount = m_channelCount;

    stbir_pixel_layout layout = static_cast<stbir_pixel_layout>(m_channelCount);
    void* result = stbir_resize_float_linear(m_data, m_width, m_height, 0, newImage->m_data,
                                             newWidth, newHeight, 0, layout);
    if (!result) [[unlikely]]
    {
        return nullptr;
    }
    return newImage;
}

bool ImageFloat32::WriteHDR(cstring_view fileName) const
{
    if (!m_data || m_width <= 0 || m_height <= 0 || !stbi::CheckChannelCount(m_channelCount))
    {
        return false;
    }
    const int result = stbi_write_hdr(fileName.c_str(), m_width, m_height, m_channelCount, m_data);
    return (result != 0); // returns 0 on failure and non-0 on success.
}

} // namespace rad
