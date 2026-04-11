#include <rad/IO/Image.h>

#include <rad/IO/Logging.h>

#include <gtest/gtest.h>

#include <cstdio> // for std::remove

using namespace rad;

void TestImageUnorm8()
{
    const int width = 256;
    const int height = 256;
    const int channels = 4;

    Ref<ImageUnorm8> image = ImageUnorm8::Create(width, height, channels);
    ASSERT_NE(image, nullptr);
    EXPECT_EQ(image->GetWidth(), width);
    EXPECT_EQ(image->GetHeight(), height);
    EXPECT_EQ(image->GetChannelCount(), channels);

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            uint8_t* pixel = image->GetPixel(x, y);
            pixel[0] = static_cast<uint8_t>(x); // R: 0 to 255 horizontally
            pixel[1] = static_cast<uint8_t>(y); // G: 0 to 255 vertically
            pixel[2] = 128;                     // B: constant
            pixel[3] = 255;                     // A: opaque
        }
    }

    EXPECT_TRUE(image->WritePNG("gradient_unorm8.png"));
    EXPECT_TRUE(image->WriteBMP("gradient_unorm8.bmp"));
    EXPECT_TRUE(image->WriteTGA("gradient_unorm8.tga"));
    EXPECT_TRUE(image->WriteJPG("gradient_unorm8.jpg", 100)); // JPEG is lossy

    image = rad::ImageUnorm8::CreateFromFile("gradient_unorm8.png", channels);
    ASSERT_NE(image, nullptr);
    EXPECT_EQ(image->GetWidth(), width);
    EXPECT_EQ(image->GetHeight(), height);
    EXPECT_EQ(image->GetChannelCount(), channels);

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            uint8_t* pixel = image->GetPixel(x, y);
            ASSERT_EQ(pixel[0], static_cast<uint8_t>(x));
            ASSERT_EQ(pixel[1], static_cast<uint8_t>(y));
            ASSERT_EQ(pixel[2], 128);
            ASSERT_EQ(pixel[3], 255);
        }
    }

    {
        Ref<ImageUnorm8> resizedImage = image->Resize(128, 128);
        ASSERT_NE(resizedImage, nullptr);
        EXPECT_EQ(resizedImage->GetWidth(), 128);
        EXPECT_EQ(resizedImage->GetHeight(), 128);
        EXPECT_TRUE(resizedImage->WritePNG("gradient_unorm8_small.png"));
    }

    {
        Ref<ImageUnorm8> resizedImage = image->Resize(512, 512);
        ASSERT_NE(resizedImage, nullptr);
        EXPECT_EQ(resizedImage->GetWidth(), 512);
        EXPECT_EQ(resizedImage->GetHeight(), 512);
        EXPECT_TRUE(resizedImage->WritePNG("gradient_unorm8_large.png"));
    }

    std::remove("gradient_unorm8.png");
    std::remove("gradient_unorm8.bmp");
    std::remove("gradient_unorm8.tga");
    std::remove("gradient_unorm8.jpg");
    std::remove("gradient_unorm8_small.png");
    std::remove("gradient_unorm8_large.png");
}

void TestImageFloat32()
{
    const int width = 256;
    const int height = 256;
    const int channels = 4;

    Ref<ImageFloat32> image = ImageFloat32::Create(width, height, channels);
    ASSERT_NE(image, nullptr);
    EXPECT_EQ(image->GetWidth(), width);
    EXPECT_EQ(image->GetHeight(), height);
    EXPECT_EQ(image->GetChannelCount(), channels);

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

    EXPECT_TRUE(image->WriteHDR("gradient_float32.hdr"));

    image = rad::ImageFloat32::CreateFromFile("gradient_float32.hdr", channels);
    ASSERT_NE(image, nullptr);
    EXPECT_EQ(image->GetWidth(), width);
    EXPECT_EQ(image->GetHeight(), height);

    // Validation
    // HDR format uses RGBE encoding which causes minor precision loss.
    float tolerance = 0.02f; // precision tolerance for RGBE format
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            float* pixel = image->GetPixel(x, y);
            EXPECT_NEAR(pixel[0], static_cast<float>(x) / static_cast<float>(width - 1), tolerance);
            EXPECT_NEAR(pixel[1], static_cast<float>(y) / static_cast<float>(height - 1),
                        tolerance);
            EXPECT_NEAR(pixel[2], 0.5f, tolerance);
        }
    }

    {
        Ref<ImageFloat32> resizedImage = image->Resize(128, 128);
        ASSERT_NE(resizedImage, nullptr);
        EXPECT_EQ(resizedImage->GetWidth(), 128);
        EXPECT_EQ(resizedImage->GetHeight(), 128);
        EXPECT_TRUE(resizedImage->WriteHDR("gradient_float32_small.hdr"));
    }

    {
        Ref<ImageFloat32> resizedImage = image->Resize(512, 512);
        ASSERT_NE(resizedImage, nullptr);
        EXPECT_EQ(resizedImage->GetWidth(), 512);
        EXPECT_EQ(resizedImage->GetHeight(), 512);
        EXPECT_TRUE(resizedImage->WriteHDR("gradient_float32_large.hdr"));
    }

    std::remove("gradient_float32.hdr");
    std::remove("gradient_float32_small.hdr");
    std::remove("gradient_float32_large.hdr");
}

TEST(IO, Image)
{
    TestImageUnorm8();
    TestImageFloat32();
}
