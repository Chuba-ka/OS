// ArrayManager.h
#ifndef ARRAY_MANAGER_H
#define ARRAY_MANAGER_H

#include <vector>

class ArrayManager
{
private:
    int num_arrays;
    int array_length;
    std::vector<std::vector<double>> arrays;

public:
    ArrayManager(int k, int l);
    void fillRandom(double min_val, double max_val);
    const std::vector<std::vector<double>> &getArrays() const;
    int getLength() const;
};

#endif // ARRAY_MANAGER_H