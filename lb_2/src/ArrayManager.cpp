#include "ArrayManager.h"
#include <cstdlib>

ArrayManager::ArrayManager(int k, int l) : num_arrays(k), array_length(l)
{
    arrays.resize(k, std::vector<double>(l, 0.0));
}

void ArrayManager::fillRandom(double min_val, double max_val)
{
    for (int i = 0; i < num_arrays; ++i)
    {
        for (int j = 0; j < array_length; ++j)
        {
            arrays[i][j] = min_val + static_cast<double>(rand()) / RAND_MAX * (max_val - min_val);
        }
    }
}

const std::vector<std::vector<double>> &ArrayManager::getArrays() const
{
    return arrays;
}

int ArrayManager::getLength() const
{
    return array_length;
}