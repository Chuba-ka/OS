#include <gtest/gtest.h>
#include "childProcessor.hpp"

// Тесты для класса ChildProcessor
TEST(ChildProcessorTest, RemoveVowelsBasic)
{
    ChildProcessor processor;
    std::string input = "Hello, World!";
    std::string expected = "Hll, Wrld!";
    EXPECT_EQ(processor.removeVowels(input), expected);
}

TEST(ChildProcessorTest, RemoveVowelsEmptyString)
{
    ChildProcessor processor;
    std::string input = "";
    std::string expected = "";
    EXPECT_EQ(processor.removeVowels(input), expected);
}

TEST(ChildProcessorTest, RemoveVowelsAllVowels)
{
    ChildProcessor processor;
    std::string input = "aeiouAEIOU";
    std::string expected = "";
    EXPECT_EQ(processor.removeVowels(input), expected);
}

TEST(ChildProcessorTest, RemoveVowelsNoVowels)
{
    ChildProcessor processor;
    std::string input = "xyz123";
    std::string expected = "xyz123";
    EXPECT_EQ(processor.removeVowels(input), expected);
}

TEST(ChildProcessorTest, RemoveVowelsMixedCase)
{
    ChildProcessor processor;
    std::string input = "AbCdEfGhIj";
    std::string expected = "bCdFGhj";
    EXPECT_EQ(processor.removeVowels(input), expected);
}