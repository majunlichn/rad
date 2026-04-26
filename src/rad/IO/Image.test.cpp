#include <rad/IO/Image.h>

#include <gtest/gtest.h>

#include <cstdio> // for std::remove

using namespace rad;

static void FillGradient(ImageUnorm8* image)
{
    int width = image->GetWidth();
    int height = image->GetHeight();
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            uint8_t* pixel = image->GetPixel(x, y);
            pixel[0] = static_cast<uint8_t>(x * (float(256) / width));  // R: 0 to 255 horizontally
            pixel[1] = static_cast<uint8_t>(y * (float(256) / height)); // G: 0 to 255 vertically
            pixel[2] = 128;                                             // B: constant
            pixel[3] = 255;                                             // A: opaque
        }
    }
}

static void VerifyPixels(const ImageUnorm8* image, int width, int height, int channels,
                         int tolerance)
{
    ASSERT_TRUE(image);
    ASSERT_EQ(image->GetWidth(), width);
    ASSERT_EQ(image->GetHeight(), height);
    ASSERT_EQ(image->GetChannelCount(), channels);
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            const uint8_t* pixel = image->GetPixel(x, y);
            ASSERT_NEAR(pixel[0], static_cast<uint8_t>(x * (float(256) / width)), tolerance);
            ASSERT_NEAR(pixel[1], static_cast<uint8_t>(y * (float(256) / height)), tolerance);
            ASSERT_NEAR(pixel[2], 128, tolerance);
            ASSERT_NEAR(pixel[3], 255, tolerance);
        }
    }
}

void TestImageUnorm8()
{
    const int width = 256;
    const int height = 256;
    const int channels = 4;

    Ref<ImageUnorm8> image = ImageUnorm8::Create(width, height, channels);
    ASSERT_TRUE(image);
    ASSERT_EQ(image->GetWidth(), width);
    ASSERT_EQ(image->GetHeight(), height);
    ASSERT_EQ(image->GetChannelCount(), channels);

    {
        SCOPED_TRACE("TestImageUnorm8: FillGradient");
        FillGradient(image.get());
        VerifyPixels(image.get(), width, height, channels, 0);
    }
    {
        SCOPED_TRACE("TestImageUnorm8: PNG read/write test");
        const char* fileName = "gradient_unorm8.png";
        std::remove(fileName);
        ASSERT_TRUE(image->WritePNG(fileName));
        image = ImageUnorm8::CreateFromFile(fileName, channels);
        ASSERT_TRUE(image);
        ASSERT_EQ(image->GetWidth(), width);
        ASSERT_EQ(image->GetHeight(), height);
        ASSERT_EQ(image->GetChannelCount(), channels);
        VerifyPixels(image.get(), width, height, channels, 0); // PNG should be lossless
    }
    {
        SCOPED_TRACE("TestImageUnorm8: BMP read/write test");
        const char* fileName = "gradient_unorm8.bmp";
        std::remove(fileName);
        ASSERT_TRUE(image->WriteBMP(fileName));
        image = ImageUnorm8::CreateFromFile(fileName, channels);
        ASSERT_TRUE(image);
        ASSERT_EQ(image->GetWidth(), width);
        ASSERT_EQ(image->GetHeight(), height);
        ASSERT_EQ(image->GetChannelCount(), channels);
        VerifyPixels(image.get(), width, height, channels, 0);
    }
    {
        SCOPED_TRACE("TestImageUnorm8: TGA read/write test");
        const char* fileName = "gradient_unorm8.tga";
        std::remove(fileName);
        ASSERT_TRUE(image->WriteTGA(fileName));
        image = ImageUnorm8::CreateFromFile(fileName, channels);
        ASSERT_TRUE(image);
        ASSERT_EQ(image->GetWidth(), width);
        ASSERT_EQ(image->GetHeight(), height);
        ASSERT_EQ(image->GetChannelCount(), channels);
        VerifyPixels(image.get(), width, height, channels, 0);
    }
    {
        SCOPED_TRACE("TestImageUnorm8: JPEG read/write test");
        const char* fileName = "gradient_unorm8.jpg";
        std::remove(fileName);
        ASSERT_TRUE(image->WriteJPG(fileName, 100)); // JPEG is lossy
        image = ImageUnorm8::CreateFromFile(fileName, channels);
        ASSERT_TRUE(image);
        ASSERT_EQ(image->GetWidth(), width);
        ASSERT_EQ(image->GetHeight(), height);
        ASSERT_EQ(image->GetChannelCount(), channels);
        VerifyPixels(image.get(), width, height, channels, 2);
    }
    {
        SCOPED_TRACE("TestImageUnorm8: resize smaller");
        const int newWidth = width / 1.5;
        const int newHeight = height / 1.5;
        Ref<ImageUnorm8> resizedImage = image->Resize(newWidth, newHeight);
        ASSERT_NE(resizedImage, nullptr);
        ASSERT_EQ(resizedImage->GetWidth(), newWidth);
        ASSERT_EQ(resizedImage->GetHeight(), newHeight);
        const char* fileName = "gradient_unorm8_smaller.png";
        std::remove(fileName);
        ASSERT_TRUE(resizedImage->WritePNG(fileName));
        VerifyPixels(resizedImage.get(), newWidth, newHeight, channels, 2);
    }
    {
        SCOPED_TRACE("TestImageUnorm8: resized larger");
        const char* fileName = "gradient_unorm8_larger.png";
        const int newWidth = width * 1.5;
        const int newHeight = height * 1.5;
        Ref<ImageUnorm8> resizedImage = image->Resize(newWidth, newHeight);
        ASSERT_NE(resizedImage, nullptr);
        ASSERT_EQ(resizedImage->GetWidth(), newWidth);
        ASSERT_EQ(resizedImage->GetHeight(), newHeight);
        std::remove(fileName);
        ASSERT_TRUE(resizedImage->WritePNG(fileName));
        VerifyPixels(resizedImage.get(), newWidth, newHeight, channels, 2);
    }
}

void FillGradient(ImageFloat32* image)
{
    int width = image->GetWidth();
    int height = image->GetHeight();
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            float* pixel = image->GetPixel(x, y);
            pixel[0] = static_cast<float>(x) / static_cast<float>(width - 1);  // R: 0.0 to 1.0
            pixel[1] = static_cast<float>(y) / static_cast<float>(height - 1); // G: 0.0 to 1.0
            pixel[2] = 0.5f;                                                   // B: 0.5
            pixel[3] = 1.0f;                                                   // A: 1.0
        }
    }
}

void VerifyPixels(const ImageFloat32* image, int width, int height, int channels, float tolerance)
{
    ASSERT_TRUE(image);
    ASSERT_EQ(image->GetWidth(), width);
    ASSERT_EQ(image->GetHeight(), height);
    ASSERT_EQ(image->GetChannelCount(), channels);
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            const float* pixel = image->GetPixel(x, y);
            ASSERT_NEAR(pixel[0], static_cast<float>(x) / static_cast<float>(width - 1),
                        tolerance); // R: 0.0 to 1.0
            ASSERT_NEAR(pixel[1], static_cast<float>(y) / static_cast<float>(height - 1),
                        tolerance);                 // G: 0.0 to 1.0
            ASSERT_NEAR(pixel[2], 0.5f, tolerance); // B: 0.5
            ASSERT_NEAR(pixel[3], 1.0f, tolerance); // A: 1.0
        }
    }
}

void TestImageFloat32()
{
    const int width = 256;
    const int height = 256;
    const int channels = 4;

    Ref<ImageFloat32> image = ImageFloat32::Create(width, height, channels);
    ASSERT_TRUE(image);
    ASSERT_EQ(image->GetWidth(), width);
    ASSERT_EQ(image->GetHeight(), height);
    ASSERT_EQ(image->GetChannelCount(), channels);
    {
        SCOPED_TRACE("TestImageFloat32: FillGradient");
        FillGradient(image.get());
        VerifyPixels(image.get(), width, height, channels, 0);
    }
    {
        SCOPED_TRACE("TestImageFloat32: HDR read/write test");
        const char* fileName = "gradient_float32.hdr";
        std::remove(fileName);
        ASSERT_TRUE(image->WriteHDR(fileName));
        Ref<ImageFloat32> imageLoaded = ImageFloat32::CreateFromFile(fileName, channels);
        ASSERT_TRUE(imageLoaded);
        ASSERT_EQ(imageLoaded->GetWidth(), width);
        ASSERT_EQ(imageLoaded->GetHeight(), height);
        // Validation
        // HDR format uses RGBE encoding which causes minor precision loss.
        VerifyPixels(imageLoaded.get(), width, height, channels, 0.02f);
    }
    {
        SCOPED_TRACE("TestImageFloat32: resize smaller");
        const int newWidth = width / 1.5;
        const int newHeight = height / 1.5;
        Ref<ImageFloat32> resizedImage = image->Resize(newWidth, newHeight);
        ASSERT_NE(resizedImage, nullptr);
        ASSERT_EQ(resizedImage->GetWidth(), newWidth);
        ASSERT_EQ(resizedImage->GetHeight(), newHeight);
        const char* fileName = "gradient_float32_smaller.hdr";
        std::remove(fileName);
        ASSERT_TRUE(resizedImage->WriteHDR(fileName));
        VerifyPixels(resizedImage.get(), newWidth, newHeight, channels, 0.02f);
    }
    {
        SCOPED_TRACE("TestImageFloat32: resize larger");
        const int newWidth = width * 1.5;
        const int newHeight = height * 1.5;
        Ref<ImageFloat32> resizedImage = image->Resize(newWidth, newHeight);
        ASSERT_NE(resizedImage, nullptr);
        ASSERT_EQ(resizedImage->GetWidth(), newWidth);
        ASSERT_EQ(resizedImage->GetHeight(), newHeight);
        const char* fileName = "gradient_float32_larger.hdr";
        std::remove(fileName);
        ASSERT_TRUE(resizedImage->WriteHDR(fileName));
        VerifyPixels(resizedImage.get(), newWidth, newHeight, channels, 0.02f);
    }
}

TEST(IO, Image)
{
    TestImageUnorm8();
    TestImageFloat32();
}
