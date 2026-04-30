#include <rad/Common/Integer.h>

#include <gtest/gtest.h>

using namespace rad;

TEST(Integer, HighLowParts)
{
    EXPECT_EQ(HighPart32<uint32_t>(0xABCD1234u), 0xABCDu);
    EXPECT_EQ(LowPart32<uint32_t>(0xABCD1234u), 0x1234u);

    EXPECT_EQ(HighPart64<uint64_t>(0x01234567'89ABCDEFULL), 0x01234567u);
    EXPECT_EQ(LowPart64<uint64_t>(0x01234567'89ABCDEFULL), 0x89ABCDEFu);
}

TEST(Integer, BitPredicates)
{
    constexpr uint32_t mask = 0b1011'0000u;

    EXPECT_TRUE(HasBits(mask, 0b0010'0000u));
    EXPECT_TRUE(HasBits(mask, 0b1001'0000u));
    EXPECT_FALSE(HasBits(mask, 0b0100'0000u));
    EXPECT_FALSE(HasBits(mask, 0b1111'0000u));

    EXPECT_TRUE(HasNoBits(mask, 0b0100'0000u));
    EXPECT_FALSE(HasNoBits(mask, 0b0010'0000u));

    EXPECT_TRUE(HasAnyBits(mask, 0b0010'0000u));
    EXPECT_TRUE(HasAnyBits(mask, 0b0110'0000u));
    EXPECT_FALSE(HasAnyBits(mask, 0b0100'0000u));
}

TEST(Integer, SignExtend)
{
    // 4-bit sign extension in an 8-bit container.
    EXPECT_EQ(SignExtend<uint8_t>(0x0u, 4), 0);
    EXPECT_EQ(SignExtend<uint8_t>(0x7u, 4), 7);
    EXPECT_EQ(SignExtend<uint8_t>(0x8u, 4), -8);
    EXPECT_EQ(SignExtend<uint8_t>(0xFu, 4), -1);

    // Edge bit widths.
    EXPECT_EQ(SignExtend<uint8_t>(0x1u, 1), -1);
    EXPECT_EQ(SignExtend<uint8_t>(0x0u, 1), 0);
    EXPECT_EQ(SignExtend<uint8_t>(0x80u, 8), -128);
    EXPECT_EQ(SignExtend<uint8_t>(0xFFu, 8), -1);

    // 12-bit sign extension in a 16-bit container.
    EXPECT_EQ(SignExtend<uint16_t>(0x7FFu, 12), 2047);
    EXPECT_EQ(SignExtend<uint16_t>(0x800u, 12), -2048);
    EXPECT_EQ(SignExtend<uint16_t>(0xFFFu, 12), -1);
}

TEST(Integer, ExtractReplaceBits)
{
    // ExtractBits: low nibble, and cross-nibble extraction.
    constexpr uint32_t value = 0xDEADBEEFu;
    EXPECT_EQ(ExtractBits<uint32_t>(value, 0, 4), 0xFu);
    EXPECT_EQ(ExtractBits<uint32_t>(value, 4, 4), 0xEu);
    EXPECT_EQ(ExtractBits<uint32_t>(value, 8, 8), 0xBEu);
    EXPECT_EQ(ExtractBits<uint32_t>(value, 16, 16), 0xDEADu);
    EXPECT_EQ(ExtractBits<uint32_t>(value, 0, BitCount<uint32_t>), value);
    EXPECT_EQ(ExtractBits<uint32_t>(value, BitCount<uint32_t>, 0), 0u);
    EXPECT_EQ(ExtractBits<uint32_t>(value, 0, 0), 0u);

    // ReplaceBits: replace a nibble and a full-width replacement.
    EXPECT_EQ(ReplaceBits<uint32_t>(0x00000000u, 0xFu, 0, 4), 0x0000000Fu);
    EXPECT_EQ(ReplaceBits<uint32_t>(0x00000000u, 0xAu, 4, 4), 0x000000A0u);
    EXPECT_EQ(ReplaceBits<uint32_t>(0xFFFFFFFFu, 0x0u, 8, 8), 0xFFFF00FFu);
    EXPECT_EQ(ReplaceBits<uint32_t>(0x11111111u, 0x22222222u, 0, BitCount<uint32_t>),
              0x22222222u);
    EXPECT_EQ(ReplaceBits<uint32_t>(0x12345678u, 0xFFFFFFFFu, BitCount<uint32_t>, 0),
              0x12345678u);
}

TEST(Integer, BitScanReverse)
{
    EXPECT_EQ(BitScanReverse(0x00000001u), 0);
    EXPECT_EQ(BitScanReverse(0x00008002u), 15);
    EXPECT_EQ(BitScanReverse(0x00010004u), 16);
    EXPECT_EQ(BitScanReverse(0x80000006u), 31);
    EXPECT_EQ(BitScanReverse(0x0000000000000001ull), 0);
    EXPECT_EQ(BitScanReverse(0x0000000080000002ull), 31);
    EXPECT_EQ(BitScanReverse(0x0000000100000004ull), 32);
    EXPECT_EQ(BitScanReverse(0x8000000000000006ull), 63);

    EXPECT_EQ(BitScanReversePortable(0x00000001u), 0);
    EXPECT_EQ(BitScanReversePortable(0x00008002u), 15);
    EXPECT_EQ(BitScanReversePortable(0x00010004u), 16);
    EXPECT_EQ(BitScanReversePortable(0x80000006u), 31);
    EXPECT_EQ(BitScanReversePortable(0x0000000000000001ull), 0);
    EXPECT_EQ(BitScanReversePortable(0x0000000080000002ull), 31);
    EXPECT_EQ(BitScanReversePortable(0x0000000100000004ull), 32);
    EXPECT_EQ(BitScanReversePortable(0x8000000000000006ull), 63);
}

TEST(Integer, CountBits)
{
    EXPECT_EQ(CountBits(0x00000000u), 0);
    EXPECT_EQ(CountBits(0x55555555u), 16);
    EXPECT_EQ(CountBits(0xAAAAAAAAu), 16);
    EXPECT_EQ(CountBits(0xFFFFFFFFu), 32);
    EXPECT_EQ(CountBits(0x0000000000000000ull), 0);
    EXPECT_EQ(CountBits(0x5555555555555555ull), 32);
    EXPECT_EQ(CountBits(0xAAAAAAAAAAAAAAAAull), 32);
    EXPECT_EQ(CountBits(0xFFFFFFFFFFFFFFFFull), 64);

    EXPECT_EQ(CountBitsPortable(0x00000000u), 0);
    EXPECT_EQ(CountBitsPortable(0x55555555u), 16);
    EXPECT_EQ(CountBitsPortable(0xAAAAAAAAu), 16);
    EXPECT_EQ(CountBitsPortable(0xFFFFFFFFu), 32);
    EXPECT_EQ(CountBitsPortable(0x0000000000000000ull), 0);
    EXPECT_EQ(CountBitsPortable(0x5555555555555555ull), 32);
    EXPECT_EQ(CountBitsPortable(0xAAAAAAAAAAAAAAAAull), 32);
    EXPECT_EQ(CountBitsPortable(0xFFFFFFFFFFFFFFFFull), 64);
}

TEST(Integer, ReverseBits)
{
    EXPECT_EQ(ReverseBits(0x00000000u), 0x00000000);
    EXPECT_EQ(ReverseBits(0x55555555u), 0xAAAAAAAA);
    EXPECT_EQ(ReverseBits(0xAAAAAAAAu), 0x55555555);
    EXPECT_EQ(ReverseBits(0xFFFFFFFFu), 0xFFFFFFFF);
    EXPECT_EQ(ReverseBits(0x0000000000000000ull), 0x0000000000000000);
    EXPECT_EQ(ReverseBits(0x5555555555555555ull), 0xAAAAAAAAAAAAAAAA);
    EXPECT_EQ(ReverseBits(0xAAAAAAAAAAAAAAAAull), 0x5555555555555555);
    EXPECT_EQ(ReverseBits(0xFFFFFFFFFFFFFFFFull), 0xFFFFFFFFFFFFFFFF);
}

TEST(Integer, DivRoundUp)
{
    EXPECT_EQ(DivRoundUp(0u, 1u), 0u);
    EXPECT_EQ(DivRoundUp(1u, 1u), 1u);
    EXPECT_EQ(DivRoundUp(2u, 1u), 2u);
    EXPECT_EQ(DivRoundUp(3u, 1u), 3u);
    EXPECT_EQ(DivRoundUp(0u, 2u), 0u);
    EXPECT_EQ(DivRoundUp(1u, 2u), 1u);
    EXPECT_EQ(DivRoundUp(2u, 2u), 1u);
    EXPECT_EQ(DivRoundUp(3u, 2u), 2u);
    EXPECT_EQ(DivRoundUp(0xFFFFFFFFFFFFFFFFull, 0x100000000ull), 0x100000000ull);
}

TEST(Integer, Pow2AndAlignment)
{
    EXPECT_FALSE(IsPow2(0u));
    EXPECT_TRUE(IsPow2(1u));
    EXPECT_TRUE(IsPow2(2u));
    EXPECT_FALSE(IsPow2(3u));
    EXPECT_TRUE(IsPow2(4u));

    // Power-of-two alignment helpers.
    EXPECT_EQ(Pow2AlignUp<uint32_t>(0u, 8u), 0u);
    EXPECT_EQ(Pow2AlignUp<uint32_t>(1u, 8u), 8u);
    EXPECT_EQ(Pow2AlignUp<uint32_t>(8u, 8u), 8u);
    EXPECT_EQ(Pow2AlignUp<uint32_t>(9u, 8u), 16u);

    EXPECT_EQ(Pow2AlignDown<uint32_t>(0u, 8u), 0u);
    EXPECT_EQ(Pow2AlignDown<uint32_t>(1u, 8u), 0u);
    EXPECT_EQ(Pow2AlignDown<uint32_t>(8u, 8u), 8u);
    EXPECT_EQ(Pow2AlignDown<uint32_t>(9u, 8u), 8u);

    // Round up/down to multiples: both pow2 and non-pow2 code paths.
    EXPECT_EQ(RoundUpToMultiple<uint32_t>(13u, 8u), 16u);
    EXPECT_EQ(RoundDownToMultiple<uint32_t>(13u, 8u), 8u);
    EXPECT_EQ(RoundUpToMultiple<uint32_t>(13u, 6u), 18u);
    EXPECT_EQ(RoundDownToMultiple<uint32_t>(13u, 6u), 12u);
    EXPECT_EQ(RoundUpToMultiple<uint32_t>(0u, 6u), 0u);
    EXPECT_EQ(RoundDownToMultiple<uint32_t>(0u, 6u), 0u);
}

TEST(Integer, Pow2Rounding)
{
    // RoundUpToNextPow2: next power-of-two even if already a power-of-two; 0 -> 1.
    EXPECT_EQ(RoundUpToNextPow2<uint32_t>(0u), 1u);
    EXPECT_EQ(RoundUpToNextPow2<uint32_t>(1u), 2u);
    EXPECT_EQ(RoundUpToNextPow2<uint32_t>(2u), 4u);
    EXPECT_EQ(RoundUpToNextPow2<uint32_t>(3u), 4u);
    EXPECT_EQ(RoundUpToNextPow2<uint32_t>(16u), 32u);

    // RoundUpToPow2: smallest pow2 not smaller than x; 0 -> 1.
    EXPECT_EQ(RoundUpToPow2<uint32_t>(0u), 1u);
    EXPECT_EQ(RoundUpToPow2<uint32_t>(1u), 1u);
    EXPECT_EQ(RoundUpToPow2<uint32_t>(2u), 2u);
    EXPECT_EQ(RoundUpToPow2<uint32_t>(3u), 4u);
    EXPECT_EQ(RoundUpToPow2<uint32_t>(16u), 16u);

    // RoundDownToPow2: 0 -> 0.
    EXPECT_EQ(RoundDownToPow2<uint32_t>(0u), 0u);
    EXPECT_EQ(RoundDownToPow2<uint32_t>(1u), 1u);
    EXPECT_EQ(RoundDownToPow2<uint32_t>(2u), 2u);
    EXPECT_EQ(RoundDownToPow2<uint32_t>(3u), 2u);
    EXPECT_EQ(RoundDownToPow2<uint32_t>(31u), 16u);

    // Portable variants should agree on a representative set.
    EXPECT_EQ(RoundUpToNextPow2Portable<uint32_t>(0u), 1u);
    EXPECT_EQ(RoundUpToNextPow2Portable<uint32_t>(1u), 2u);
    EXPECT_EQ(RoundUpToNextPow2Portable<uint32_t>(2u), 4u);
    EXPECT_EQ(RoundUpToNextPow2Portable<uint32_t>(16u), 32u);

    EXPECT_EQ(RoundUpToPow2Portable<uint32_t>(0u), 1u);
    EXPECT_EQ(RoundUpToPow2Portable<uint32_t>(1u), 1u);
    EXPECT_EQ(RoundUpToPow2Portable<uint32_t>(2u), 2u);
    EXPECT_EQ(RoundUpToPow2Portable<uint32_t>(3u), 4u);
    EXPECT_EQ(RoundUpToPow2Portable<uint32_t>(16u), 16u);

    EXPECT_EQ(RoundUpToNextPow2Portable<uint64_t>(0u), 1u);
    EXPECT_EQ(RoundUpToNextPow2Portable<uint64_t>(1u), 2u);
    EXPECT_EQ(RoundUpToNextPow2Portable<uint64_t>(2u), 4u);
    EXPECT_EQ(RoundUpToNextPow2Portable<uint64_t>(16u), 32u);

    EXPECT_EQ(RoundUpToPow2Portable<uint64_t>(0u), 1u);
    EXPECT_EQ(RoundUpToPow2Portable<uint64_t>(1u), 1u);
    EXPECT_EQ(RoundUpToPow2Portable<uint64_t>(2u), 2u);
    EXPECT_EQ(RoundUpToPow2Portable<uint64_t>(3u), 4u);
    EXPECT_EQ(RoundUpToPow2Portable<uint64_t>(16u), 16u);
}
