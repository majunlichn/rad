#include <rad/Common/Integer.h>

#include <iostream>

#include <gtest/gtest.h>

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
    EXPECT_EQ(rad::DivRoundUp(4u, 1u), 4u);
    EXPECT_EQ(rad::DivRoundUp(5u, 1u), 5u);
    EXPECT_EQ(rad::DivRoundUp(0u, 2u), 0u);
    EXPECT_EQ(rad::DivRoundUp(1u, 2u), 1u);
    EXPECT_EQ(rad::DivRoundUp(2u, 2u), 1u);
    EXPECT_EQ(rad::DivRoundUp(3u, 2u), 2u);
    EXPECT_EQ(rad::DivRoundUp(4u, 2u), 2u);
    EXPECT_EQ(rad::DivRoundUp(5u, 2u), 3u);
    EXPECT_EQ(rad::DivRoundUp(0xFFFFFFFFFFFFFFFFull, 0x100000000ull), 0x100000000ull);
}

void TestSignExtend()
{
    EXPECT_EQ(rad::SignExtend<uint32_t>(0x000000u, 24), 0);
    EXPECT_EQ(rad::SignExtend<uint32_t>(0xFFFFFFu, 24), -1);
    EXPECT_EQ(rad::SignExtend<uint32_t>(0x800000u, 24), -8388608);
    EXPECT_EQ(rad::SignExtend<uint32_t>(0x7FFFFFu, 24), 8388607);
}

void TestExtractBits()
{
    EXPECT_EQ(rad::ExtractBits<uint32_t>(0b1010, 0, 3), 0b010u);
    EXPECT_EQ(rad::ExtractBits<uint32_t>(0b1010, 1, 3), 0b101u);
    EXPECT_EQ(rad::ExtractBits<uint32_t>(0x12345678u, 24, 8), 0x12u);
    EXPECT_EQ(rad::ExtractBits<uint32_t>(0x12345678u, 20, 8), 0x23u);
    EXPECT_EQ(rad::ExtractBits<uint32_t>(0x12345678u, 16, 8), 0x34u);
    EXPECT_EQ(rad::ExtractBits<uint32_t>(0x12345678u, 12, 8), 0x45u);
    EXPECT_EQ(rad::ExtractBits<uint32_t>(0x12345678u, 8, 8), 0x56u);
    EXPECT_EQ(rad::ExtractBits<uint32_t>(0x12345678u, 4, 8), 0x67u);
    EXPECT_EQ(rad::ExtractBits<uint32_t>(0x12345678u, 0, 8), 0x78u);
    EXPECT_EQ(rad::ExtractBits<uint32_t>(0x12345678u, 0, 32), 0x12345678u);
    EXPECT_EQ(rad::ExtractBits<uint32_t>(0x12345678u, 0, 0), 0x0u);
}

void TestReplaceBits()
{
    EXPECT_EQ(rad::ReplaceBits<uint32_t>(0b1010, 0b10, 1, 2), 0b1100);
    EXPECT_EQ(rad::ReplaceBits<uint32_t>(0xFFFFFFFFu, 0x00u, 0, 8), 0xFFFFFF00);
    EXPECT_EQ(rad::ReplaceBits<uint32_t>(0xFFFFFFFFu, 0x00u, 8, 8), 0xFFFF00FF);
    EXPECT_EQ(rad::ReplaceBits<uint32_t>(0xFFFFFFFFu, 0x00u, 16, 8), 0xFF00FFFF);
    EXPECT_EQ(rad::ReplaceBits<uint32_t>(0xFFFFFFFFu, 0x00u, 24, 8), 0x00FFFFFF);
    EXPECT_EQ(rad::ReplaceBits<uint32_t>(0xFFFFFFFFu, 0x12345678u, 0, 0), 0xFFFFFFFFu);
    EXPECT_EQ(rad::ReplaceBits<uint32_t>(0xFFFFFFFFu, 0x12345678u, 8, 0), 0xFFFFFFFFu);
    EXPECT_EQ(rad::ReplaceBits<uint32_t>(0xFFFFFFFFu, 0x12345678u, 0, 32), 0x12345678u);
}

TEST(Common, Integer)
{
    TestBitScanReverse32();
    TestCountBits();
    TestReverseBits();
    TestDivRoundUp();
    TestSignExtend();
    TestExtractBits();
    TestReplaceBits();
}
