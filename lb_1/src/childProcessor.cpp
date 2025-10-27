#include "childProcessor.hpp"
#include <algorithm>
#include <cctype>

std::string ChildProcessor::removeVowels(const std::string &input)
{
    std::string result = input;
    std::string vowels = "aeiouAEIOU";
    result.erase(std::remove_if(result.begin(), result.end(), [&](char c)
                                { return vowels.find(c) != std::string::npos; }),
                 result.end());
    return result;
}