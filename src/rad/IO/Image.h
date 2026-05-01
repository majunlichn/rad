#pragma once

#include <rad/Common/RefCounted.h>
#include <rad/Common/String.h>

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <utility>

namespace rad
{

class ImageUnorm8 : public RefCounted<ImageUnorm8>
{
public:
    ImageUnorm8();
    ~ImageUnorm8();

    ImageUnorm8(const ImageUnorm8&) = delete;
    ImageUnorm8& operator=(const ImageUnorm8&) = delete;

    // Move constructor
    ImageUnorm8(ImageUnorm8&& other) noexcept :
        m_data(std::exchange(other.m_data, nullptr)),
        m_width(std::exchange(other.m_width, 0)),
        m_height(std::exchange(other.m_height, 0)),
        m_channelCount(std::exchange(other.m_channelCount, 0))
    {
    }

    // Move assignment
    ImageUnorm8& operator=(ImageUnorm8&& other) noexcept
    {
        if (this != &other)
        {
            Destroy();
            m_data = std::exchange(other.m_data, nullptr);
            m_width = std::exchange(other.m_width, 0);
            m_height = std::exchange(other.m_height, 0);
            m_channelCount = std::exchange(other.m_channelCount, 0);
        }
        return *this;
    }

    void Destroy();

    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    int GetChannelCount() const { return m_channelCount; }
    const uint8_t* GetData() const { return m_data; }

    /// Byte length of the packed buffer: width * height * channelCount.
    size_t GetDataSize() const noexcept
    {
        return static_cast<size_t>(m_width) * static_cast<size_t>(m_height) *
               static_cast<size_t>(m_channelCount);
    }

    uint8_t* GetPixel(size_t x, size_t y)
    {
        assert(m_data != nullptr);
        assert(m_width > 0 && m_height > 0 && m_channelCount > 0);
        assert(x < static_cast<size_t>(m_width) && y < static_cast<size_t>(m_height));
        size_t offset = (y * size_t(m_width) + x) * size_t(m_channelCount);
        return (m_data + offset);
    }

    const uint8_t* GetPixel(size_t x, size_t y) const
    {
        assert(m_data != nullptr);
        assert(m_width > 0 && m_height > 0 && m_channelCount > 0);
        assert(x < static_cast<size_t>(m_width) && y < static_cast<size_t>(m_height));
        size_t offset = (y * size_t(m_width) + x) * size_t(m_channelCount);
        return (m_data + offset);
    }

    /// Allocates width * height * channelCount bytes; \p clearValue is the memset byte ([0, 255]).
    /// Use Fill on an existing allocation when you want to change that fill later.
    bool Allocate(int width, int height, int channelCount = 4, int clearValue = 0);

    /// Sets every byte of the buffer to \p value. Fails if not allocated.
    bool Fill(uint8_t value);

    static bool GetFileInfo(cstring_view fileName, int* width, int* height, int* channelCount);
    /// stb_image HDR->LDR tone-mapping globals (not thread-safe if threads use different values).
    static void SetHdrToLdrGamma(float gamma);
    /// See SetHdrToLdrGamma.
    static void SetHdrToLdrScale(float scale);
    bool LoadFromFile(cstring_view fileName, int channelCount = 4);
    bool LoadFromMemory(const void* buffer, size_t bufferSize, int channelCount = 4);
    static Ref<ImageUnorm8> Create(int width, int height, int channelCount = 4, int clearValue = 0);
    static Ref<ImageUnorm8> CreateFromFile(cstring_view fileName, int channelCount = 4);
    static Ref<ImageUnorm8> CreateFromMemory(const void* buffer, size_t bufferSize,
                                             int channelCount = 4);

    /// Resamples to new dimensions. On failure returns null Ref after rolling back allocation (the
    /// caller never receives a partly initialized image).
    Ref<ImageUnorm8> Resize(int newWidth, int newHeight) const;

    /// Packed rows: \p strideBytesPerRow must be >= \c m_width * \c m_channelCount (STB default).
    bool WritePNG(cstring_view fileName) const;
    /// Same pixels as full-image WritePNG but with an explicit row stride (bytes between row starts).
    bool WritePNG(cstring_view fileName, int strideBytesPerRow) const;
    /// Sub-rectangle [left, right) x [top, bottom). Writes (right - left) x (bottom - top) pixels.
    /// Row stride passed to stbi_write_png is the parent image stride (m_width * m_channelCount),
    /// not the cropped width alone.
    bool WritePNG(cstring_view fileName, int left, int top, int right, int bottom) const;
    bool WriteBMP(cstring_view fileName) const;
    bool WriteTGA(cstring_view fileName) const;
    /// JPEG ignores alpha in input data; \p quality must be in [1, 100].
    bool WriteJPG(cstring_view fileName, int quality) const;

private:
    uint8_t* m_data = nullptr;
    int m_width = 0;
    int m_height = 0;
    int m_channelCount = 0;

}; // class ImageUnorm8

// Linear float color channels (loads via stbi_loadf; writes Radiance .hdr with WriteHDR).
class ImageFloat32 : public rad::RefCounted<ImageFloat32>
{
public:
    ImageFloat32();
    ~ImageFloat32();

    ImageFloat32(const ImageFloat32&) = delete;
    ImageFloat32& operator=(const ImageFloat32&) = delete;

    // Move constructor
    ImageFloat32(ImageFloat32&& other) noexcept :
        m_data(std::exchange(other.m_data, nullptr)),
        m_width(std::exchange(other.m_width, 0)),
        m_height(std::exchange(other.m_height, 0)),
        m_channelCount(std::exchange(other.m_channelCount, 0))
    {
    }

    // Move assignment
    ImageFloat32& operator=(ImageFloat32&& other) noexcept
    {
        if (this != &other)
        {
            Destroy();
            m_data = std::exchange(other.m_data, nullptr);
            m_width = std::exchange(other.m_width, 0);
            m_height = std::exchange(other.m_height, 0);
            m_channelCount = std::exchange(other.m_channelCount, 0);
        }
        return *this;
    }

    void Destroy();

    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    int GetChannelCount() const { return m_channelCount; }
    const float* GetData() const { return m_data; }

    /// Number of stored floats: width * height * channelCount.
    size_t GetDataSize() const noexcept
    {
        return static_cast<size_t>(m_width) * static_cast<size_t>(m_height) *
               static_cast<size_t>(m_channelCount);
    }

    float* GetPixel(size_t x, size_t y)
    {
        assert(m_data != nullptr);
        assert(m_width > 0 && m_height > 0 && m_channelCount > 0);
        assert(x < static_cast<size_t>(m_width) && y < static_cast<size_t>(m_height));
        size_t offset = (y * size_t(m_width) + x) * size_t(m_channelCount);
        return (m_data + offset);
    }

    const float* GetPixel(size_t x, size_t y) const
    {
        assert(m_data != nullptr);
        assert(m_width > 0 && m_height > 0 && m_channelCount > 0);
        assert(x < static_cast<size_t>(m_width) && y < static_cast<size_t>(m_height));
        size_t offset = (y * size_t(m_width) + x) * size_t(m_channelCount);
        return (m_data + offset);
    }

    bool Allocate(int width, int height, int channelCount = 4, float clearValue = 0);

    /// Sets every float in the buffer to \p value. Fails if not allocated.
    bool Fill(float value);

    static bool GetFileInfo(cstring_view fileName, int* width, int* height, int* channelCount);
    /// stb_image LDR->HDR globals when promoting 8-bit sources to float (process-wide; see SetHdrToLdrGamma).
    static void SetLdrToHdrGamma(float gamma);
    /// See SetLdrToHdrGamma.
    static void SetLdrToHdrScale(float scale);
    bool LoadFromFile(cstring_view fileName, int channelCount = 4);
    bool LoadFromMemory(const void* buffer, size_t bufferSize, int channelCount = 4);

    static Ref<ImageFloat32> Create(int width, int height, int channelCount = 4,
                                    float clearValue = 0.0f);
    static Ref<ImageFloat32> CreateFromFile(cstring_view fileName, int channelCount = 4);
    static Ref<ImageFloat32> CreateFromMemory(const void* buffer, size_t bufferSize,
                                              int channelCount = 4);

    /// On failure returns null Ref after rolling back allocation (same guarantee as ImageUnorm8::Resize).
    Ref<ImageFloat32> Resize(int newWidth, int newHeight) const;

    /// Radiance HDR (.hdr): expects linear RGB as floats; alpha is discarded. Single-channel data is
    /// replicated to all three RGB channels.
    bool WriteHDR(cstring_view fileName) const;

private:
    float* m_data = nullptr;
    int m_width = 0;
    int m_height = 0;
    int m_channelCount = 0;

}; // class ImageFloat32

} // namespace rad
