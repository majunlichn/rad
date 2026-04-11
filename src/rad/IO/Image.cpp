#include <rad/System/MemoryDebug.h> // Must be included before the other headers.

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

#include <cstring>

namespace rad
{

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
    size_t dataSize = static_cast<size_t>(width) * height * channelCount;
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

bool ImageUnorm8::GetFileInfo(cstring_view fileName, int* width, int* height, int* channelCount)
{
    int ok = stbi_info(fileName.c_str(), width, height, channelCount);
    return (ok != 0);
}

bool ImageUnorm8::LoadFromFile(cstring_view fileName, int channelCount, float gamma, float scale)
{
    Destroy();
    assert(m_data == nullptr);
    stbi_hdr_to_ldr_gamma(gamma);
    stbi_hdr_to_ldr_scale(scale);
    int originalChannelCount = 0;
    m_data = (uint8_t*)stbi_load(fileName.c_str(), &m_width, &m_height, &originalChannelCount,
                                 channelCount);
    if (m_data)
    {
        m_channelCount = (channelCount > 0) ? channelCount : originalChannelCount;
        return true;
    }
    return (m_data != nullptr);
}

bool ImageUnorm8::LoadFromMemory(const void* buffer, size_t bufferSize, int channelCount,
                                 float gamma, float scale)
{
    Destroy();
    assert(m_data == nullptr);
    stbi_hdr_to_ldr_gamma(gamma);
    stbi_hdr_to_ldr_scale(scale);
    int originalChannelCount = 0;
    m_data =
        (uint8_t*)stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(buffer), bufferSize,
                                        &m_width, &m_height, &originalChannelCount, channelCount);
    if (m_data)
    {
        m_channelCount = (channelCount > 0) ? channelCount : originalChannelCount;
        return true;
    }
    return (m_data != nullptr);
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

Ref<ImageUnorm8> ImageUnorm8::CreateFromFile(cstring_view fileName, int channelCount, float gamma,
                                             float scale)
{
    Ref<ImageUnorm8> newImage = new ImageUnorm8();
    if (newImage->LoadFromFile(fileName, channelCount, gamma, scale))
    {
        return newImage;
    }
    else
    {
        return nullptr;
    }
}

Ref<ImageUnorm8> ImageUnorm8::CreateFromMemory(const void* buffer, size_t bufferSize,
                                               int channelCount, float gamma, float scale)
{
    Ref<ImageUnorm8> newImage = new ImageUnorm8();
    if (newImage->LoadFromMemory(buffer, bufferSize, channelCount, gamma, scale))
    {
        return newImage;
    }
    else
    {
        return nullptr;
    }
}

rad::Ref<ImageUnorm8> ImageUnorm8::Resize(int newWidth, int newHeight) const
{
    if (!m_data || newWidth <= 0 || newHeight <= 0)
    {
        return nullptr;
    }

    rad::Ref<ImageUnorm8> newImage = new ImageUnorm8();
    size_t newSize = static_cast<size_t>(newWidth) * newHeight * m_channelCount;
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
    int result = stbi_write_png(fileName.c_str(), m_width, m_height, m_channelCount, m_data,
                                m_width * m_channelCount);
    return (result != 0); // returns 0 on failure and non-0 on success.
}

bool ImageUnorm8::WritePNG(cstring_view fileName, int left, int top, int right, int bottom) const
{
    assert(left > 0 && left < m_width && left < right && right < m_width);
    assert(top > 0 && top < m_height && top < bottom && bottom < m_height);
    int width = right - left;
    int height = bottom - top;
    int offset = (top * m_width + left) * m_channelCount;
    int result = stbi_write_png(fileName.c_str(), width, height, m_channelCount, m_data + offset,
                                m_width * m_channelCount);
    return (result != 0); // returns 0 on failure and non-0 on success.
}

bool ImageUnorm8::WriteBMP(cstring_view fileName) const
{
    int result = stbi_write_bmp(fileName.c_str(), m_width, m_height, m_channelCount, m_data);
    return (result != 0); // returns 0 on failure and non-0 on success.
}

bool ImageUnorm8::WriteTGA(cstring_view fileName) const
{
    int result = stbi_write_tga(fileName.c_str(), m_width, m_height, m_channelCount, m_data);
    return (result != 0); // returns 0 on failure and non-0 on success.
}

bool ImageUnorm8::WriteJPG(cstring_view fileName, int quality) const
{
    int result =
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
    assert((m_data == nullptr) && (width > 0) && (height > 0) && (channelCount > 0));
    float* data = nullptr;
    size_t sizeInBytes = size_t(width) * size_t(height) * size_t(channelCount) * sizeof(float);
    data = (float*)malloc(sizeInBytes);

    if (data)
    {
        m_data = data;
        std::fill_n(m_data, sizeInBytes / sizeof(float), clearValue);
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

bool ImageFloat32::GetFileInfo(cstring_view fileName, int* width, int* height, int* channelCount)
{
    int ok = stbi_info(fileName.c_str(), width, height, channelCount);
    return (ok != 0);
}

bool ImageFloat32::LoadFromFile(cstring_view filename, int channelCount, float gamma, float scale)
{
    Destroy();
    assert(m_data == nullptr);
    stbi_ldr_to_hdr_gamma(gamma);
    stbi_ldr_to_hdr_scale(scale);
    int originalChannelCount = 0;
    m_data = stbi_loadf(filename.c_str(), &m_width, &m_height, &originalChannelCount, channelCount);
    if (m_data)
    {
        m_channelCount = (channelCount > 0) ? channelCount : originalChannelCount;
        return true;
    }
    else
    {
        return false;
    }
}

bool ImageFloat32::LoadFromMemory(const void* buffer, size_t bufferSize, int channelCount,
                                  float gamma, float scale)
{
    Destroy();
    assert(m_data == nullptr);
    stbi_ldr_to_hdr_gamma(gamma);
    stbi_ldr_to_hdr_scale(scale);
    int originalChannelCount = 0;
    m_data = stbi_loadf_from_memory(reinterpret_cast<const stbi_uc*>(buffer), bufferSize, &m_width,
                                    &m_height, &originalChannelCount, channelCount);
    if (m_data)
    {
        m_channelCount = (channelCount > 0) ? channelCount : originalChannelCount;
        return true;
    }
    return (m_data != nullptr);
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

Ref<ImageFloat32> ImageFloat32::CreateFromFile(cstring_view fileName, int channelCount, float gamma,
                                               float scale)
{
    Ref<ImageFloat32> image = new ImageFloat32();
    if (image->LoadFromFile(fileName, channelCount, gamma, scale))
    {
        return image;
    }
    else
    {
        return nullptr;
    }
}

Ref<ImageFloat32> ImageFloat32::CreateFromMemory(const void* buffer, size_t bufferSize,
                                                 int channelCount, float gamma, float scale)
{
    Ref<ImageFloat32> newImage = new ImageFloat32();
    if (newImage->LoadFromMemory(buffer, bufferSize, channelCount, gamma, scale))
    {
        return newImage;
    }
    else
    {
        return nullptr;
    }
}

rad::Ref<ImageFloat32> ImageFloat32::Resize(int newWidth, int newHeight) const
{
    if (!m_data || newWidth <= 0 || newHeight <= 0)
    {
        return nullptr;
    }

    rad::Ref<ImageFloat32> newImage = new ImageFloat32();
    size_t newSizeInBytes =
        static_cast<size_t>(newWidth) * newHeight * m_channelCount * sizeof(float);
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

bool ImageFloat32::WriteHDR(cstring_view filename) const
{
    int result = stbi_write_hdr(filename.c_str(), m_width, m_height, m_channelCount, m_data);
    return (result != 0); // returns 0 on failure and non-0 on success.
}

} // namespace rad
