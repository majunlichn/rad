#pragma once

#include <rad/Common/RefCounted.h>
#include <rad/Common/String.h>

#include <utility>

namespace rad
{

class ImageUnorm8 : public RefCounted<ImageUnorm8>
{
    uint8_t* m_data = nullptr;
    int m_width = 0;
    int m_height = 0;
    int m_channelCount = 0;

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
    size_t GetDataSize() const { return static_cast<size_t>(m_width) * m_height * m_channelCount; }

    uint8_t* GetPixel(size_t x, size_t y)
    {
        size_t offset = (y * size_t(m_width) + x) * size_t(m_channelCount);
        return (m_data + offset);
    }

    const uint8_t* GetPixel(size_t x, size_t y) const
    {
        size_t offset = (y * size_t(m_width) + x) * size_t(m_channelCount);
        return (m_data + offset);
    }

    bool Allocate(int width, int height, int channelCount = 4, int clearValue = 0);
    static bool GetFileInfo(cstring_view fileName, int* width, int* height, int* channelCount);
    bool LoadFromFile(cstring_view fileName, int channelCount = 4, float gamma = 2.2f,
                      float scale = 1.0f);
    bool LoadFromMemory(const void* buffer, size_t bufferSize, int channelCount = 4,
                        float gamma = 2.2f, float scale = 1.0f);
    static Ref<ImageUnorm8> Create(int width, int height, int channelCount = 4, int clearValue = 0);
    static Ref<ImageUnorm8> CreateFromFile(cstring_view fileName, int channelCount = 4,
                                           float gamma = 2.2f, float scale = 1.0f);
    static Ref<ImageUnorm8> CreateFromMemory(const void* buffer, size_t bufferSize,
                                             int channelCount = 4, float gamma = 2.2f,
                                             float scale = 1.0f);

    rad::Ref<ImageUnorm8> Resize(int newWidth, int newHeight) const;

    bool WritePNG(cstring_view fileName) const;
    // Write a sub-region.
    bool WritePNG(cstring_view fileName, int left, int top, int right, int bottom) const;
    bool WriteBMP(cstring_view fileName) const;
    bool WriteTGA(cstring_view fileName) const;
    // JPEG does ignore alpha channels in input data; quality is between 1 and 100.
    // Higher quality looks better but results in a bigger image.
    bool WriteJPG(cstring_view fileName, int quality) const;

}; // class ImageUnorm8

// Float32 HDR data.
class ImageFloat32 : public rad::RefCounted<ImageFloat32>
{
    float* m_data = nullptr;
    int m_width = 0;
    int m_height = 0;
    int m_channelCount = 0;

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
    size_t GetDataSize() const { return static_cast<size_t>(m_width) * m_height * m_channelCount; }

    float* GetPixel(size_t x, size_t y)
    {
        size_t offset = (y * size_t(m_width) + x) * size_t(m_channelCount);
        return (m_data + offset);
    }

    const float* GetPixel(size_t x, size_t y) const
    {
        size_t offset = (y * size_t(m_width) + x) * size_t(m_channelCount);
        return (m_data + offset);
    }

    bool Allocate(int width, int height, int channelCount = 4, float clearValue = 0);
    static bool GetFileInfo(cstring_view fileName, int* width, int* height, int* channelCount);
    bool LoadFromFile(cstring_view fileName, int channelCount = 4, float gamma = 2.2f,
                      float scale = 1.0f);
    bool LoadFromMemory(const void* buffer, size_t bufferSize, int channelCount = 4,
                        float gamma = 2.2f, float scale = 1.0f);

    static Ref<ImageFloat32> Create(int width, int height, int channelCount = 4,
                                    float clearValue = 0.0f);
    static Ref<ImageFloat32> CreateFromFile(cstring_view fileName, int channelCount = 4,
                                            float gamma = 2.2f, float scale = 1.0f);
    static Ref<ImageFloat32> CreateFromMemory(const void* buffer, size_t bufferSize,
                                              int channelCount = 4, float gamma = 2.2f,
                                              float scale = 1.0f);

    Ref<ImageFloat32> Resize(int newWidth, int newHeight) const;

    // HDR expects linear float rgb(e) data, alpha (if provided) is discarded,
    // and for monochrome data it is replicated across all three channels.
    bool WriteHDR(cstring_view filename) const;

}; // class ImageFloat32

} // namespace rad
