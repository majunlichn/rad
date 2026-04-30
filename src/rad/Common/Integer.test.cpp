#include <rad/Common/Integer.h>

#include <iostream>

#include <gtest/gtest.h>

void TestHighLowParts()
{
    EXPECT_EQ(rad::HighPart32<uint32_t>(0xABCD1234u), 0xABCDu);
    EXPECT_EQ(rad::LowPart32<uint32_t>(0xABCD1234u), 0x1234u);

    EXPECT_EQ(rad::HighPart64<uint64_t>(0x01234567'89ABCDEFULL), 0x01234567u);
    EXPECT_EQ(rad::LowPart64<uint64_t>(0x01234567'89ABCDEFULL), 0x89ABCDEFu);
}

void TestBitPredicates()
{
    constexpr uint32_t mask = 0b1011'0000u;

    EXPECT_TRUE(rad::HasBits(mask, 0b0010'0000u));
    EXPECT_TRUE(rad::HasBits(mask, 0b1001'0000u));
    EXPECT_FALSE(rad::HasBits(mask, 0b0100'0000u));
    EXPECT_FALSE(rad::HasBits(mask, 0b1111'0000u));

    EXPECT_TRUE(rad::HasNoBits(mask, 0b0100'0000u));
    EXPECT_FALSE(rad::HasNoBits(mask, 0b0010'0000u));

    EXPECT_TRUE(rad::HasAnyBits(mask, 0b0010'0000u));
    EXPECT_TRUE(rad::HasAnyBits(mask, 0b0110'0000u));
    EXPECT_FALSE(rad::HasAnyBits(mask, 0b0100'0000u));
}

void TestSignExtend()
{
    // 4-bit sign extension in an 8-bit container.
    EXPECT_EQ(rad::SignExtend<uint8_t>(0x0u, 4), 0);
    EXPECT_EQ(rad::SignExtend<uint8_t>(0x7u, 4), 7);
    EXPECT_EQ(rad::SignExtend<uint8_t>(0x8u, 4), -8);
    EXPECT_EQ(rad::SignExtend<uint8_t>(0xFu, 4), -1);

    // Edge bit widths.
    EXPECT_EQ(rad::SignExtend<uint8_t>(0x1u, 1), -1);
    EXPECT_EQ(rad::SignExtend<uint8_t>(0x0u, 1), 0);
    EXPECT_EQ(rad::SignExtend<uint8_t>(0x80u, 8), -128);
    EXPECT_EQ(rad::SignExtend<uint8_t>(0xFFu, 8), -1);

    // 12-bit sign extension in a 16-bit container.
    EXPECT_EQ(rad::SignExtend<uint16_t>(0x7FFu, 12), 2047);
    EXPECT_EQ(rad::SignExtend<uint16_t>(0x800u, 12), -2048);
    EXPECT_EQ(rad::SignExtend<uint16_t>(0xFFFu, 12), -1);
}

void TestExtractReplaceBits()
{
    // ExtractBits: low nibble, and cross-nibble extraction.
    constexpr uint32_t value = 0xDEADBEEFu;
    EXPECT_EQ(rad::ExtractBits<uint32_t>(value, 0, 4), 0xFu);
    EXPECT_EQ(rad::ExtractBits<uint32_t>(value, 4, 4), 0xEu);
    EXPECT_EQ(rad::ExtractBits<uint32_t>(value, 8, 8), 0xBEu);
    EXPECT_EQ(rad::ExtractBits<uint32_t>(value, 16, 16), 0xDEADu);
    EXPECT_EQ(rad::ExtractBits<uint32_t>(value, 0, rad::BitCount<uint32_t>), value);

    // ReplaceBits: replace a nibble and a full-width replacement.
    EXPECT_EQ(rad::ReplaceBits<uint32_t>(0x00000000u, 0xFu, 0, 4), 0x0000000Fu);
    EXPECT_EQ(rad::ReplaceBits<uint32_t>(0x00000000u, 0xAu, 4, 4), 0x000000A0u);
    EXPECT_EQ(rad::ReplaceBits<uint32_t>(0xFFFFFFFFu, 0x0u, 8, 8), 0xFFFF00FFu);
    EXPECT_EQ(rad::ReplaceBits<uint32_t>(0x11111111u, 0x22222222u, 0, rad::BitCount<uint32_t>), 0x22222222u);
}

void TestBitScanReverse32()
{
    EXPECT_EQ(rad::BitScanReverse(0x00000001u), 0);
    EXPECT_EQ(rad::BitScanReverse(0x00008002u), 15);
    EXPECT_EQ(rad::BitScanReverse(0x00010004u), 16);
    EXPECT_EQ(rad::BitScanReverse(0x80000006u), 31);
    EXPECT_EQ(rad::BitScanReverse(0x0000000000000001ull), 0);
    EXPECT_EQ(rad::BitScanReverse(0x0000000080000002ull), 31);
    EXPECT_EQ(rad::BitScanReverse(0x0000000100000004ull), 32);
    EXPECT_EQ(rad::BitScanReverse(0x8000000000000006ull), 63);

    EXPECT_EQ(rad::BitScanReversePortable(0x00000001u), 0);
    EXPECT_EQ(rad::BitScanReversePortable(0x00008002u), 15);
    EXPECT_EQ(rad::BitScanReversePortable(0x00010004u), 16);
    EXPECT_EQ(rad::BitScanReversePortable(0x80000006u), 31);
    EXPECT_EQ(rad::BitScanReversePortable(0x0000000000000001ull), 0);
    EXPECT_EQ(rad::BitScanReversePortable(0x0000000080000002ull), 31);
    EXPECT_EQ(rad::BitScanReversePortable(0x0000000100000004ull), 32);
    EXPECT_EQ(rad::BitScanReversePortable(0x8000000000000006ull), 63);
}

void TestCountBits()
{
    EXPECT_EQ(rad::CountBits(0x00000000u), 0);
    EXPECT_EQ(rad::CountBits(0x55555555u), 16);
    EXPECT_EQ(rad::CountBits(0xAAAAAAAAu), 16);
    EXPECT_EQ(rad::CountBits(0xFFFFFFFFu), 32);
    EXPECT_EQ(rad::CountBits(0x0000000000000000ull), 0);
    EXPECT_EQ(rad::CountBits(0x5555555555555555ull), 32);
    EXPECT_EQ(rad::CountBits(0xAAAAAAAAAAAAAAAAull), 32);
    EXPECT_EQ(rad::CountBits(0xFFFFFFFFFFFFFFFFull), 64);

    EXPECT_EQ(rad::CountBitsPortable(0x00000000u), 0);
    EXPECT_EQ(rad::CountBitsPortable(0x55555555u), 16);
    EXPECT_EQ(rad::CountBitsPortable(0xAAAAAAAAu), 16);
    EXPECT_EQ(rad::CountBitsPortable(0xFFFFFFFFu), 32);
    EXPECT_EQ(rad::CountBitsPortable(0x0000000000000000ull), 0);
    EXPECT_EQ(rad::CountBitsPortable(0x5555555555555555ull), 32);
    EXPECT_EQ(rad::CountBitsPortable(0xAAAAAAAAAAAAAAAAull), 32);
    EXPECT_EQ(rad::CountBitsPortable(0xFFFFFFFFFFFFFFFFull), 64);
}

void TestReverseBits()
{
    EXPECT_EQ(rad::ReverseBits(0x00000000u), 0x00000000);
    EXPECT_EQ(rad::ReverseBits(0x55555555u), 0xAAAAAAAA);
    EXPECT_EQ(rad::ReverseBits(0xAAAAAAAAu), 0x55555555);
    EXPECT_EQ(rad::ReverseBits(0xFFFFFFFFu), 0xFFFFFFFF);
    EXPECT_EQ(rad::ReverseBits(0x0000000000000000ull), 0x0000000000000000);
    EXPECT_EQ(rad::ReverseBits(0x5555555555555555ull), 0xAAAAAAAAAAAAAAAA);
    EXPECT_EQ(rad::ReverseBits(0xAAAAAAAAAAAAAAAAull), 0x5555555555555555);
    EXPECT_EQ(rad::ReverseBits(0xFFFFFFFFFFFFFFFFull), 0xFFFFFFFFFFFFFFFF);
}

void TestDivRoundUp()
{
    EXPECT_EQ(rad::DivRoundUp(0u, 1u), 0u);
    EXPECT_EQ(rad::DivRoundUp(1u, 1u), 1u);
    EXPECT_EQ(rad::DivRoundUp(2u, 1u), 2u);
    EXPECT_EQ(rad::DivRoundUp(3u, 1u), 3u);
    EXPECT_EQ(rad::DivRoundUp(0u, 2u), 0u);
    EXPECT_EQ(rad::DivRoundUp(1u, 2u), 1u);
    EXPECT_EQ(rad::DivRoundUp(2u, 2u), 1u);
    EXPECT_EQ(rad::DivRoundUp(3u, 2u), 2u);
    EXPECT_EQ(rad::DivRoundUp(0xFFFFFFFFFFFFFFFFull, 0x100000000ull), 0x100000000ull);
}

void TestPow2AndAlignment()
{
    EXPECT_FALSE(rad::IsPow2(0u));
    EXPECT_TRUE(rad::IsPow2(1u));
    EXPECT_TRUE(rad::IsPow2(2u));
    EXPECT_FALSE(rad::IsPow2(3u));
    EXPECT_TRUE(rad::IsPow2(4u));

    // Power-of-two alignment helpers.
    EXPECT_EQ(rad::Pow2AlignUp<uint32_t>(0u, 8u), 0u);
    EXPECT_EQ(rad::Pow2AlignUp<uint32_t>(1u, 8u), 8u);
    EXPECT_EQ(rad::Pow2AlignUp<uint32_t>(8u, 8u), 8u);
    EXPECT_EQ(rad::Pow2AlignUp<uint32_t>(9u, 8u), 16u);

    EXPECT_EQ(rad::Pow2AlignDown<uint32_t>(0u, 8u), 0u);
    EXPECT_EQ(rad::Pow2AlignDown<uint32_t>(1u, 8u), 0u);
    EXPECT_EQ(rad::Pow2AlignDown<uint32_t>(8u, 8u), 8u);
    EXPECT_EQ(rad::Pow2AlignDown<uint32_t>(9u, 8u), 8u);

    // Round up/down to multiples: both pow2 and non-pow2 code paths.
    EXPECT_EQ(rad::RoundUpToMultiple<uint32_t>(13u, 8u), 16u);
    EXPECT_EQ(rad::RoundDownToMultiple<uint32_t>(13u, 8u), 8u);
    EXPECT_EQ(rad::RoundUpToMultiple<uint32_t>(13u, 6u), 18u);
    EXPECT_EQ(rad::RoundDownToMultiple<uint32_t>(13u, 6u), 12u);
    EXPECT_EQ(rad::RoundUpToMultiple<uint32_t>(0u, 6u), 0u);
    EXPECT_EQ(rad::RoundDownToMultiple<uint32_t>(0u, 6u), 0u);
}

void TestPow2Rounding()
{
    // RoundUpToNextPow2: next power-of-two even if already a power-of-two; 0 -> 1.
    EXPECT_EQ(rad::RoundUpToNextPow2<uint32_t>(0u), 1u);
    EXPECT_EQ(rad::RoundUpToNextPow2<uint32_t>(1u), 2u);
    EXPECT_EQ(rad::RoundUpToNextPow2<uint32_t>(2u), 4u);
    EXPECT_EQ(rad::RoundUpToNextPow2<uint32_t>(3u), 4u);
    EXPECT_EQ(rad::RoundUpToNextPow2<uint32_t>(16u), 32u);

    // RoundUpToPow2: smallest pow2 not smaller than x; 0 -> 1.
    EXPECT_EQ(rad::RoundUpToPow2<uint32_t>(0u), 1u);
    EXPECT_EQ(rad::RoundUpToPow2<uint32_t>(1u), 1u);
    EXPECT_EQ(rad::RoundUpToPow2<uint32_t>(2u), 2u);
    EXPECT_EQ(rad::RoundUpToPow2<uint32_t>(3u), 4u);
    EXPECT_EQ(rad::RoundUpToPow2<uint32_t>(16u), 16u);

    // RoundDownToPow2: 0 -> 0.
    EXPECT_EQ(rad::RoundDownToPow2<uint32_t>(0u), 0u);
    EXPECT_EQ(rad::RoundDownToPow2<uint32_t>(1u), 1u);
    EXPECT_EQ(rad::RoundDownToPow2<uint32_t>(2u), 2u);
    EXPECT_EQ(rad::RoundDownToPow2<uint32_t>(3u), 2u);
    EXPECT_EQ(rad::RoundDownToPow2<uint32_t>(31u), 16u);

    // Portable variants should agree on a representative set.
    EXPECT_EQ(rad::RoundUpToNextPow2Portable<uint32_t>(0u), 1u);
    EXPECT_EQ(rad::RoundUpToNextPow2Portable<uint32_t>(1u), 2u);
    EXPECT_EQ(rad::RoundUpToNextPow2Portable<uint32_t>(2u), 4u);
    EXPECT_EQ(rad::RoundUpToNextPow2Portable<uint32_t>(16u), 32u);

    EXPECT_EQ(rad::RoundUpToPow2Portable<uint32_t>(0u), 1u);
    EXPECT_EQ(rad::RoundUpToPow2Portable<uint32_t>(1u), 1u);
    EXPECT_EQ(rad::RoundUpToPow2Portable<uint32_t>(2u), 2u);
    EXPECT_EQ(rad::RoundUpToPow2Portable<uint32_t>(3u), 4u);
    EXPECT_EQ(rad::RoundUpToPow2Portable<uint32_t>(16u), 16u);

    EXPECT_EQ(rad::RoundUpToNextPow2Portable<uint64_t>(0u), 1u);
    EXPECT_EQ(rad::RoundUpToNextPow2Portable<uint64_t>(1u), 2u);
    EXPECT_EQ(rad::RoundUpToNextPow2Portable<uint64_t>(2u), 4u);
    EXPECT_EQ(rad::RoundUpToNextPow2Portable<uint64_t>(16u), 32u);

    EXPECT_EQ(rad::RoundUpToPow2Portable<uint64_t>(0u), 1u);
    EXPECT_EQ(rad::RoundUpToPow2Portable<uint64_t>(1u), 1u);
    EXPECT_EQ(rad::RoundUpToPow2Portable<uint64_t>(2u), 2u);
    EXPECT_EQ(rad::RoundUpToPow2Portable<uint64_t>(3u), 4u);
    EXPECT_EQ(rad::RoundUpToPow2Portable<uint64_t>(16u), 16u);
}

TEST(Common, Integer)
{
    TestHighLowParts();
    TestBitPredicates();
    TestSignExtend();
    TestExtractReplaceBits();
    TestBitScanReverse32();
    TestCountBits();
    TestReverseBits();
    TestDivRoundUp();
    TestPow2AndAlignment();
    TestPow2Rounding();
}
