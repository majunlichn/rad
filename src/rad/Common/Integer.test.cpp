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
    EXPECT_EQ(rad::DivRoundUp(0u, 2u), 0u);
    EXPECT_EQ(rad::DivRoundUp(1u, 2u), 1u);
    EXPECT_EQ(rad::DivRoundUp(2u, 2u), 1u);
    EXPECT_EQ(rad::DivRoundUp(3u, 2u), 2u);
    EXPECT_EQ(rad::DivRoundUp(0xFFFFFFFFFFFFFFFFull, 0x100000000ull), 0x100000000ull);
}

TEST(Common, Integer)
{
    TestBitScanReverse32();
    TestCountBits();
    TestReverseBits();
    TestDivRoundUp();
}
