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

    EXPECT_EQ(rad::BitScanReverse32Portable(0x00000001u), 0);
    EXPECT_EQ(rad::BitScanReverse32Portable(0x00008002u), 15);
    EXPECT_EQ(rad::BitScanReverse32Portable(0x00010004u), 16);
    EXPECT_EQ(rad::BitScanReverse32Portable(0x80000006u), 31);
    EXPECT_EQ(rad::BitScanReverse64Portable(0x0000000000000001ull), 0);
    EXPECT_EQ(rad::BitScanReverse64Portable(0x0000000080000002ull), 31);
    EXPECT_EQ(rad::BitScanReverse64Portable(0x0000000100000004ull), 32);
    EXPECT_EQ(rad::BitScanReverse64Portable(0x8000000000000006ull), 63);
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
    EXPECT_EQ(rad::ReverseBits32(0x00000000), 0x00000000);
    EXPECT_EQ(rad::ReverseBits32(0x55555555), 0xAAAAAAAA);
    EXPECT_EQ(rad::ReverseBits32(0xAAAAAAAA), 0x55555555);
    EXPECT_EQ(rad::ReverseBits32(0xFFFFFFFF), 0xFFFFFFFF);
    EXPECT_EQ(rad::ReverseBits64(0x0000000000000000), 0x0000000000000000);
    EXPECT_EQ(rad::ReverseBits64(0x5555555555555555), 0xAAAAAAAAAAAAAAAA);
    EXPECT_EQ(rad::ReverseBits64(0xAAAAAAAAAAAAAAAA), 0x5555555555555555);
    EXPECT_EQ(rad::ReverseBits64(0xFFFFFFFFFFFFFFFF), 0xFFFFFFFFFFFFFFFF);
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

TEST(Common, Integer)
{
    TestBitScanReverse32();
    TestCountBits();
    TestReverseBits();
    TestDivRoundUp();
}
