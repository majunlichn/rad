#include <rad/IO/Image.h>

#include <gtest/gtest.h>

#include <cstdint>
#include <filesystem>
#include <vector>

using namespace rad;

namespace
{

void RemoveImage(const char* path)
{
    std::error_code ec;
    (void)std::filesystem::remove(path, ec);
    ASSERT_FALSE(ec) << "remove failed for \"" << path << "\": " << ec.message();
}

void FillGradient(ImageUnorm8* image)
{
    const int width = image->GetWidth();
    const int height = image->GetHeight();
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            uint8_t* pixel = image->GetPixel(static_cast<size_t>(x), static_cast<size_t>(y));
            pixel[0] = static_cast<uint8_t>(x * (256.f / static_cast<float>(width)));
            pixel[1] = static_cast<uint8_t>(y * (256.f / static_cast<float>(height)));
            pixel[2] = 128;
            pixel[3] = 255;
        }
    }
}

void VerifyPixels(const ImageUnorm8* image, int width, int height, int channelCount, int tolerance)
{
    ASSERT_TRUE(image);
    ASSERT_EQ(image->GetWidth(), width);
    ASSERT_EQ(image->GetHeight(), height);
    ASSERT_EQ(image->GetChannelCount(), channelCount);
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            const uint8_t* pixel = image->GetPixel(static_cast<size_t>(x), static_cast<size_t>(y));
            ASSERT_NEAR(
                static_cast<double>(pixel[0]),
                static_cast<double>(static_cast<uint8_t>(x * (256.f / static_cast<float>(width)))),
                tolerance);
            ASSERT_NEAR(
                static_cast<double>(pixel[1]),
                static_cast<double>(static_cast<uint8_t>(y * (256.f / static_cast<float>(height)))),
                tolerance);
            ASSERT_NEAR(static_cast<double>(pixel[2]), 128.0, tolerance);
            ASSERT_NEAR(static_cast<double>(pixel[3]), 255.0, tolerance);
        }
    }
}

void FillGradient(ImageFloat32* image)
{
    const int width = image->GetWidth();
    const int height = image->GetHeight();
    ASSERT_GT(width, 1);
    ASSERT_GT(height, 1);
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            float* pixel = image->GetPixel(static_cast<size_t>(x), static_cast<size_t>(y));
            pixel[0] = static_cast<float>(x) / static_cast<float>(width - 1);
            pixel[1] = static_cast<float>(y) / static_cast<float>(height - 1);
            pixel[2] = 0.5f;
            pixel[3] = 1.0f;
        }
    }
}

void VerifyPixels(const ImageFloat32* image, int width, int height, int channelCount,
                  float tolerance)
{
    ASSERT_TRUE(image);
    ASSERT_EQ(image->GetWidth(), width);
    ASSERT_EQ(image->GetHeight(), height);
    ASSERT_EQ(image->GetChannelCount(), channelCount);
    ASSERT_GT(width, 1);
    ASSERT_GT(height, 1);
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            const float* pixel = image->GetPixel(static_cast<size_t>(x), static_cast<size_t>(y));
            ASSERT_NEAR(pixel[0], static_cast<float>(x) / static_cast<float>(width - 1), tolerance);
            ASSERT_NEAR(pixel[1], static_cast<float>(y) / static_cast<float>(height - 1),
                        tolerance);
            ASSERT_NEAR(pixel[2], 0.5f, tolerance);
            ASSERT_NEAR(pixel[3], 1.0f, tolerance);
        }
    }
}

} // namespace

TEST(Image, InvalidDimensions)
{
    EXPECT_EQ(ImageUnorm8::Create(0, 64, 4), nullptr);
    EXPECT_EQ(ImageUnorm8::Create(64, 0, 4), nullptr);
    EXPECT_EQ(ImageUnorm8::Create(64, 64, 0), nullptr);
    EXPECT_EQ(ImageUnorm8::Create(64, 64, 5), nullptr);
    EXPECT_EQ(ImageFloat32::Create(0, 64, 4), nullptr);
    EXPECT_EQ(ImageFloat32::Create(64, 0, 4), nullptr);
    EXPECT_EQ(ImageFloat32::Create(64, 64, 0), nullptr);
    EXPECT_EQ(ImageFloat32::Create(64, 64, 5), nullptr);
}

TEST(Image, InvalidInputs)
{
    constexpr const char* emptyWritePngName = "empty_write.png";
    constexpr const char* emptyWriteJpgName = "empty_write.jpg";
    constexpr const char* emptyWriteHdrName = "empty_write.hdr";
    constexpr const char* invalidJpegQualityName = "bad_quality.jpg";

    ImageUnorm8 emptyUnorm8;
    EXPECT_EQ(emptyUnorm8.GetDataSize(), 0u);
    EXPECT_FALSE(emptyUnorm8.WritePNG(emptyWritePngName));
    EXPECT_FALSE(emptyUnorm8.WriteJPG(emptyWriteJpgName, 90));
    EXPECT_FALSE(emptyUnorm8.Fill(0));

    ImageFloat32 emptyFloat32;
    EXPECT_EQ(emptyFloat32.GetDataSize(), 0u);
    EXPECT_FALSE(emptyFloat32.WriteHDR(emptyWriteHdrName));
    EXPECT_FALSE(emptyFloat32.Fill(0.0f));

    Ref<ImageUnorm8> imageSmallRgba = ImageUnorm8::Create(8, 8, 4);
    ASSERT_TRUE(imageSmallRgba);
    EXPECT_EQ(imageSmallRgba->GetDataSize(), 256u);
    EXPECT_FALSE(imageSmallRgba->WriteJPG(invalidJpegQualityName, 0));
    EXPECT_FALSE(imageSmallRgba->WriteJPG(invalidJpegQualityName, 101));
    EXPECT_EQ(imageSmallRgba->Resize(0, 8), nullptr);
    EXPECT_EQ(imageSmallRgba->Resize(8, 0), nullptr);

    ImageUnorm8 imageInvalidClearValue;
    EXPECT_FALSE(imageInvalidClearValue.Allocate(4, 4, 4, 256));

    const std::vector<std::uint8_t> truncatedPngSignature = {
        0x89,
        static_cast<std::uint8_t>('P'),
        static_cast<std::uint8_t>('N'),
        static_cast<std::uint8_t>('G'),
    };
    ImageUnorm8 imageLoadFromMemoryUnorm8;
    EXPECT_FALSE(imageLoadFromMemoryUnorm8.LoadFromMemory(truncatedPngSignature.data(),
                                                          truncatedPngSignature.size(), 4));
    EXPECT_EQ(imageLoadFromMemoryUnorm8.GetWidth(), 0);
    EXPECT_EQ(imageLoadFromMemoryUnorm8.GetHeight(), 0);

    ImageFloat32 imageLoadFromMemoryFloat32;
    EXPECT_FALSE(imageLoadFromMemoryFloat32.LoadFromMemory(truncatedPngSignature.data(),
                                                           truncatedPngSignature.size(), 4));
    EXPECT_EQ(imageLoadFromMemoryFloat32.GetWidth(), 0);
    EXPECT_EQ(imageLoadFromMemoryFloat32.GetHeight(), 0);

    EXPECT_FALSE(imageLoadFromMemoryUnorm8.LoadFromMemory(truncatedPngSignature.data(),
                                                          truncatedPngSignature.size(), 5));

    ASSERT_TRUE(imageSmallRgba->Fill(77));
    EXPECT_EQ(imageSmallRgba->GetPixel(0, 0)[0], 77);
}

TEST(Image, MetadataMissingFileAndGrayChannel)
{
    constexpr const char* nonexistentPngName = "rad_image_test_nonexistent.png";
    int w = -1;
    int h = -1;
    int c = -1;
    EXPECT_FALSE(ImageUnorm8::GetFileInfo(nonexistentPngName, &w, &h, &c));
    EXPECT_EQ(ImageUnorm8::CreateFromFile(nonexistentPngName, 4), nullptr);
    EXPECT_EQ(ImageFloat32::CreateFromFile(nonexistentPngName, 4), nullptr);

    constexpr const char* graySingleChannelPngName = "rad_image_test_gray1.png";
    RemoveImage(graySingleChannelPngName);
    Ref<ImageUnorm8> grayImage = ImageUnorm8::Create(16, 16, 1, 42);
    ASSERT_TRUE(grayImage);
    ASSERT_TRUE(grayImage->WritePNG(graySingleChannelPngName));
    ASSERT_TRUE(ImageUnorm8::GetFileInfo(graySingleChannelPngName, &w, &h, &c));
    EXPECT_EQ(w, 16);
    EXPECT_EQ(h, 16);
    EXPECT_GE(c, 1);

    Ref<ImageUnorm8> imageLoadedExplicitSingleChannel =
        ImageUnorm8::CreateFromFile(graySingleChannelPngName, 1);
    ASSERT_TRUE(imageLoadedExplicitSingleChannel);
    EXPECT_EQ(imageLoadedExplicitSingleChannel->GetWidth(), 16);
    EXPECT_EQ(imageLoadedExplicitSingleChannel->GetHeight(), 16);
    EXPECT_EQ(imageLoadedExplicitSingleChannel->GetChannelCount(), 1);
    EXPECT_EQ(imageLoadedExplicitSingleChannel->GetPixel(0, 0)[0], 42u);

    Ref<ImageUnorm8> imageLoadedImplicitChannelCount =
        ImageUnorm8::CreateFromFile(graySingleChannelPngName, 0);
    ASSERT_TRUE(imageLoadedImplicitChannelCount);
    EXPECT_EQ(imageLoadedImplicitChannelCount->GetWidth(), 16);
    EXPECT_EQ(imageLoadedImplicitChannelCount->GetHeight(), 16);
    EXPECT_GE(imageLoadedImplicitChannelCount->GetChannelCount(), 1);
}

TEST(Image, MoveFillAndJpegEdge)
{
    ImageUnorm8 imageUnorm8Allocated;
    ASSERT_TRUE(imageUnorm8Allocated.Allocate(3, 4, 2, 10));
    EXPECT_EQ(imageUnorm8Allocated.GetDataSize(), 24u);
    ImageUnorm8 imageUnorm8MoveConstructed(std::move(imageUnorm8Allocated));
    EXPECT_EQ(imageUnorm8MoveConstructed.GetWidth(), 3);
    EXPECT_EQ(imageUnorm8MoveConstructed.GetHeight(), 4);
    EXPECT_EQ(imageUnorm8MoveConstructed.GetChannelCount(), 2);
    ASSERT_TRUE(imageUnorm8MoveConstructed.Fill(99));
    EXPECT_EQ(imageUnorm8MoveConstructed.GetPixel(0, 0)[0], 99u);
    EXPECT_EQ(imageUnorm8MoveConstructed.GetPixel(0, 0)[1], 99u);

    ImageUnorm8 imageUnorm8MoveAssignTarget;
    ASSERT_TRUE(imageUnorm8MoveAssignTarget.Allocate(1, 1, 4, 0));
    imageUnorm8MoveAssignTarget = std::move(imageUnorm8MoveConstructed);
    EXPECT_EQ(imageUnorm8MoveAssignTarget.GetWidth(), 3);
    EXPECT_EQ(imageUnorm8MoveAssignTarget.GetDataSize(), 24u);

    Ref<ImageUnorm8> imageJpegSource = ImageUnorm8::Create(32, 32, 4, 200);
    ASSERT_TRUE(imageJpegSource);
    constexpr const char* jpegMinQualityName = "rad_image_test_minq.jpg";
    RemoveImage(jpegMinQualityName);
    ASSERT_TRUE(imageJpegSource->WriteJPG(jpegMinQualityName, 1));
    Ref<ImageUnorm8> imageJpegRoundTrip = ImageUnorm8::CreateFromFile(jpegMinQualityName, 4);
    ASSERT_TRUE(imageJpegRoundTrip);
    EXPECT_EQ(imageJpegRoundTrip->GetWidth(), 32);

    Ref<ImageFloat32> imageFloatFill = ImageFloat32::Create(4, 4, 4, 0.f);
    ASSERT_TRUE(imageFloatFill);
    ASSERT_TRUE(imageFloatFill->Fill(2.5f));
    EXPECT_FLOAT_EQ(imageFloatFill->GetPixel(2, 2)[2], 2.5f);
}

TEST(Image, Unorm8ReadWrite)
{
    constexpr int jpegLossyRoundTripTolerance = 2;

    const int width = 256;
    const int height = 256;
    const int channelCount = 4;

    Ref<ImageUnorm8> image = ImageUnorm8::Create(width, height, channelCount);
    ASSERT_TRUE(image);
    ASSERT_EQ(image->GetWidth(), width);
    ASSERT_EQ(image->GetHeight(), height);
    ASSERT_EQ(image->GetChannelCount(), channelCount);

    {
        SCOPED_TRACE("FillGradient");
        FillGradient(image.get());
        VerifyPixels(image.get(), width, height, channelCount, 0);
    }
    {
        SCOPED_TRACE("PNG read/write");
        constexpr const char* unorm8GradientPngName = "gradient_unorm8.png";
        RemoveImage(unorm8GradientPngName);
        ASSERT_TRUE(image->WritePNG(unorm8GradientPngName));
        image = ImageUnorm8::CreateFromFile(unorm8GradientPngName, channelCount);
        ASSERT_TRUE(image);
        ASSERT_EQ(image->GetWidth(), width);
        ASSERT_EQ(image->GetHeight(), height);
        ASSERT_EQ(image->GetChannelCount(), channelCount);
        VerifyPixels(image.get(), width, height, channelCount, 0);
    }
    {
        SCOPED_TRACE("PNG sub-rectangle [0,w/2)x[0,h/2)");
        constexpr const char* unorm8GradientCropPngName = "gradient_unorm8_crop.png";
        RemoveImage(unorm8GradientCropPngName);
        const int cw = width / 2;
        const int ch = height / 2;
        ASSERT_TRUE(image->WritePNG(unorm8GradientCropPngName, 0, 0, cw, ch));
        Ref<ImageUnorm8> imageQuarterCrop =
            ImageUnorm8::CreateFromFile(unorm8GradientCropPngName, channelCount);
        ASSERT_TRUE(imageQuarterCrop);
        ASSERT_EQ(imageQuarterCrop->GetWidth(), cw);
        ASSERT_EQ(imageQuarterCrop->GetHeight(), ch);
        for (int y = 0; y < ch; ++y)
        {
            for (int x = 0; x < cw; ++x)
            {
                const uint8_t* pixel =
                    imageQuarterCrop->GetPixel(static_cast<size_t>(x), static_cast<size_t>(y));
                ASSERT_NEAR(static_cast<double>(pixel[0]),
                            static_cast<double>(
                                static_cast<uint8_t>(x * (256.f / static_cast<float>(width)))),
                            0.0);
                ASSERT_NEAR(static_cast<double>(pixel[1]),
                            static_cast<double>(
                                static_cast<uint8_t>(y * (256.f / static_cast<float>(height)))),
                            0.0);
                ASSERT_NEAR(static_cast<double>(pixel[2]), 128.0, 0.0);
                ASSERT_NEAR(static_cast<double>(pixel[3]), 255.0, 0.0);
            }
        }
    }
    {
        SCOPED_TRACE("invalid PNG sub-rectangle bounds");
        constexpr const char* unorm8GradientInvalidCropPngName = "gradient_bad_bounds.png";
        ASSERT_FALSE(image->WritePNG(unorm8GradientInvalidCropPngName, -1, 0, 10, 10));
        ASSERT_FALSE(image->WritePNG(unorm8GradientInvalidCropPngName, 0, 0, width + 1, height));
        ASSERT_FALSE(image->WritePNG(unorm8GradientInvalidCropPngName, 0, 0, width, height + 1));
        ASSERT_FALSE(image->WritePNG(unorm8GradientInvalidCropPngName, width - 1, height - 1,
                                     width - 1, height)); // empty rect (width 0)
    }
    {
        SCOPED_TRACE("PNG crop full extent [0,width)x[0,height)");
        constexpr const char* unorm8GradientCropFullPngName = "gradient_unorm8_crop_full.png";
        RemoveImage(unorm8GradientCropFullPngName);
        ASSERT_TRUE(image->WritePNG(unorm8GradientCropFullPngName, 0, 0, width, height));
        Ref<ImageUnorm8> imageFullExtentCrop =
            ImageUnorm8::CreateFromFile(unorm8GradientCropFullPngName, channelCount);
        ASSERT_TRUE(imageFullExtentCrop);
        ASSERT_EQ(imageFullExtentCrop->GetWidth(), width);
        ASSERT_EQ(imageFullExtentCrop->GetHeight(), height);
        VerifyPixels(imageFullExtentCrop.get(), width, height, channelCount, 0);
    }
    {
        SCOPED_TRACE("PNG crop 1-pixel-wide strip");
        constexpr const char* unorm8GradientCropStripPngName = "gradient_unorm8_crop_strip.png";
        RemoveImage(unorm8GradientCropStripPngName);
        ASSERT_TRUE(image->WritePNG(unorm8GradientCropStripPngName, 0, 0, 1, height));
        Ref<ImageUnorm8> imageVerticalStripCrop =
            ImageUnorm8::CreateFromFile(unorm8GradientCropStripPngName, channelCount);
        ASSERT_TRUE(imageVerticalStripCrop);
        ASSERT_EQ(imageVerticalStripCrop->GetWidth(), 1);
        ASSERT_EQ(imageVerticalStripCrop->GetHeight(), height);
        for (int y = 0; y < height; ++y)
        {
            const uint8_t* pixel = imageVerticalStripCrop->GetPixel(0, static_cast<size_t>(y));
            ASSERT_NEAR(
                static_cast<double>(pixel[0]),
                static_cast<double>(static_cast<uint8_t>(0 * (256.f / static_cast<float>(width)))),
                0.0);
            ASSERT_NEAR(
                static_cast<double>(pixel[1]),
                static_cast<double>(static_cast<uint8_t>(y * (256.f / static_cast<float>(height)))),
                0.0);
            ASSERT_NEAR(static_cast<double>(pixel[2]), 128.0, 0.0);
            ASSERT_NEAR(static_cast<double>(pixel[3]), 255.0, 0.0);
        }
    }
    {
        SCOPED_TRACE("WritePNG explicit packed stride");
        constexpr const char* unorm8GradientStridePackedPngName = "gradient_stride_packed.png";
        RemoveImage(unorm8GradientStridePackedPngName);
        const int packedStride = image->GetWidth() * image->GetChannelCount();
        ASSERT_TRUE(image->WritePNG(unorm8GradientStridePackedPngName, packedStride));
        Ref<ImageUnorm8> imagePackedStrideRoundTrip =
            ImageUnorm8::CreateFromFile(unorm8GradientStridePackedPngName, channelCount);
        ASSERT_TRUE(imagePackedStrideRoundTrip);
        VerifyPixels(imagePackedStrideRoundTrip.get(), width, height, channelCount, 0);
    }
    {
        SCOPED_TRACE("WritePNG stride too small");
        constexpr const char* unorm8StrideRejectedPngName = "stride_rejected.png";
        const int packedStride = image->GetWidth() * image->GetChannelCount();
        ASSERT_FALSE(image->WritePNG(unorm8StrideRejectedPngName, packedStride - 1));
    }
    {
        SCOPED_TRACE("BMP read/write");
        constexpr const char* unorm8GradientBmpName = "gradient_unorm8.bmp";
        RemoveImage(unorm8GradientBmpName);
        ASSERT_TRUE(image->WriteBMP(unorm8GradientBmpName));
        image = ImageUnorm8::CreateFromFile(unorm8GradientBmpName, channelCount);
        ASSERT_TRUE(image);
        ASSERT_EQ(image->GetWidth(), width);
        ASSERT_EQ(image->GetHeight(), height);
        ASSERT_EQ(image->GetChannelCount(), channelCount);
        VerifyPixels(image.get(), width, height, channelCount, 0);
    }
    {
        SCOPED_TRACE("TGA read/write");
        constexpr const char* unorm8GradientTgaName = "gradient_unorm8.tga";
        RemoveImage(unorm8GradientTgaName);
        ASSERT_TRUE(image->WriteTGA(unorm8GradientTgaName));
        image = ImageUnorm8::CreateFromFile(unorm8GradientTgaName, channelCount);
        ASSERT_TRUE(image);
        ASSERT_EQ(image->GetWidth(), width);
        ASSERT_EQ(image->GetHeight(), height);
        ASSERT_EQ(image->GetChannelCount(), channelCount);
        VerifyPixels(image.get(), width, height, channelCount, 0);
    }
    {
        SCOPED_TRACE("JPEG read/write (lossy)");
        constexpr const char* unorm8GradientJpgName = "gradient_unorm8.jpg";
        RemoveImage(unorm8GradientJpgName);
        ASSERT_TRUE(image->WriteJPG(unorm8GradientJpgName, 100));
        image = ImageUnorm8::CreateFromFile(unorm8GradientJpgName, channelCount);
        ASSERT_TRUE(image);
        ASSERT_EQ(image->GetWidth(), width);
        ASSERT_EQ(image->GetHeight(), height);
        ASSERT_EQ(image->GetChannelCount(), channelCount);
        VerifyPixels(image.get(), width, height, channelCount, jpegLossyRoundTripTolerance);
    }
    {
        SCOPED_TRACE("resize smaller");
        const int newWidth = static_cast<int>(static_cast<double>(width) / 1.5);
        const int newHeight = static_cast<int>(static_cast<double>(height) / 1.5);
        Ref<ImageUnorm8> imageResizedSmaller = image->Resize(newWidth, newHeight);
        ASSERT_NE(imageResizedSmaller, nullptr);
        ASSERT_EQ(imageResizedSmaller->GetWidth(), newWidth);
        ASSERT_EQ(imageResizedSmaller->GetHeight(), newHeight);
        constexpr const char* unorm8ResizeSmallerPngName = "gradient_unorm8_smaller.png";
        RemoveImage(unorm8ResizeSmallerPngName);
        ASSERT_TRUE(imageResizedSmaller->WritePNG(unorm8ResizeSmallerPngName));
        VerifyPixels(imageResizedSmaller.get(), newWidth, newHeight, channelCount,
                     jpegLossyRoundTripTolerance);
    }
    {
        SCOPED_TRACE("resize larger");
        constexpr const char* unorm8ResizeLargerPngName = "gradient_unorm8_larger.png";
        const int newWidth = static_cast<int>(static_cast<double>(width) * 1.5);
        const int newHeight = static_cast<int>(static_cast<double>(height) * 1.5);
        Ref<ImageUnorm8> imageResizedLarger = image->Resize(newWidth, newHeight);
        ASSERT_NE(imageResizedLarger, nullptr);
        ASSERT_EQ(imageResizedLarger->GetWidth(), newWidth);
        ASSERT_EQ(imageResizedLarger->GetHeight(), newHeight);
        RemoveImage(unorm8ResizeLargerPngName);
        ASSERT_TRUE(imageResizedLarger->WritePNG(unorm8ResizeLargerPngName));
        VerifyPixels(imageResizedLarger.get(), newWidth, newHeight, channelCount,
                     jpegLossyRoundTripTolerance);
    }
}

TEST(Image, Float32ReadWrite)
{
    constexpr float hdrRoundTripTolerance = 0.02f;

    const int width = 256;
    const int height = 256;
    const int channelCount = 4;

    Ref<ImageFloat32> image = ImageFloat32::Create(width, height, channelCount);
    ASSERT_TRUE(image);
    ASSERT_EQ(image->GetWidth(), width);
    ASSERT_EQ(image->GetHeight(), height);
    ASSERT_EQ(image->GetChannelCount(), channelCount);

    {
        SCOPED_TRACE("FillGradient");
        FillGradient(image.get());
        VerifyPixels(image.get(), width, height, channelCount, 0.f);
    }
    {
        SCOPED_TRACE("HDR read/write");
        constexpr const char* float32GradientHdrName = "gradient_float32.hdr";
        RemoveImage(float32GradientHdrName);
        ASSERT_TRUE(image->WriteHDR(float32GradientHdrName));
        Ref<ImageFloat32> imageHdrRoundTrip =
            ImageFloat32::CreateFromFile(float32GradientHdrName, channelCount);
        ASSERT_TRUE(imageHdrRoundTrip);
        ASSERT_EQ(imageHdrRoundTrip->GetWidth(), width);
        ASSERT_EQ(imageHdrRoundTrip->GetHeight(), height);
        VerifyPixels(imageHdrRoundTrip.get(), width, height, channelCount, hdrRoundTripTolerance);
    }
    {
        SCOPED_TRACE("resize smaller");
        const int newWidth = static_cast<int>(static_cast<double>(width) / 1.5);
        const int newHeight = static_cast<int>(static_cast<double>(height) / 1.5);
        Ref<ImageFloat32> imageResizedSmaller = image->Resize(newWidth, newHeight);
        ASSERT_NE(imageResizedSmaller, nullptr);
        ASSERT_EQ(imageResizedSmaller->GetWidth(), newWidth);
        ASSERT_EQ(imageResizedSmaller->GetHeight(), newHeight);
        constexpr const char* float32ResizeSmallerHdrName = "gradient_float32_smaller.hdr";
        RemoveImage(float32ResizeSmallerHdrName);
        ASSERT_TRUE(imageResizedSmaller->WriteHDR(float32ResizeSmallerHdrName));
        VerifyPixels(imageResizedSmaller.get(), newWidth, newHeight, channelCount,
                     hdrRoundTripTolerance);
    }
    {
        SCOPED_TRACE("resize larger");
        const int newWidth = static_cast<int>(static_cast<double>(width) * 1.5);
        const int newHeight = static_cast<int>(static_cast<double>(height) * 1.5);
        Ref<ImageFloat32> imageResizedLarger = image->Resize(newWidth, newHeight);
        ASSERT_NE(imageResizedLarger, nullptr);
        ASSERT_EQ(imageResizedLarger->GetWidth(), newWidth);
        ASSERT_EQ(imageResizedLarger->GetHeight(), newHeight);
        constexpr const char* float32ResizeLargerHdrName = "gradient_float32_larger.hdr";
        RemoveImage(float32ResizeLargerHdrName);
        ASSERT_TRUE(imageResizedLarger->WriteHDR(float32ResizeLargerHdrName));
        VerifyPixels(imageResizedLarger.get(), newWidth, newHeight, channelCount,
                     hdrRoundTripTolerance);
    }
}
