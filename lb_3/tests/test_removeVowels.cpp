#include <gtest/gtest.h>
#include "childProcessor.hpp"

TEST(RemoveVowelsTest, Basic)
{
    ChildProcessor p;
    EXPECT_EQ(p.removeVowels("hello"), "hll");
    EXPECT_EQ(p.removeVowels("AEIOU"), "");
}

TEST(RemoveVowelsTest, EdgeCases)
{
    ChildProcessor p;
    EXPECT_EQ(p.removeVowels(""), "");
    EXPECT_EQ(p.removeVowels("bcd"), "bcd");
    EXPECT_EQ(p.removeVowels("aEiOu"), "");
    EXPECT_EQ(p.removeVowels("test\n"), "tst\n");
}