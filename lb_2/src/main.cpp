// main.cpp
#include <iostream>
#include <vector>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include "ArrayManager.h"
#include "ParallelSummer.h"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Использование: " << argv[0] << " <max_threads>" << std::endl;
        return 1;
    }

    int max_threads = std::atoi(argv[1]);
    if (max_threads <= 0)
    {
        std::cerr << "Максимальное количество потоков должно быть положительным." << std::endl;
        return 1;
    }

    std::srand(std::time(nullptr));

    const int K = 10;
    const int L = 1000000;

    ArrayManager array_manager(K, L);
    array_manager.fillRandom(0.0, 100.0);

    std::cout << "PID процесса: " << getpid() << std::endl;
    std::cout << "Для демонстрации количества потоков используйте команду: ps -T -p " << getpid() << std::endl;
    std::cout << "Или top -H -p " << getpid() << std::endl;

    ParallelSummer summer(array_manager.getArrays(), L, max_threads);
    summer.sumArrays();

    const std::vector<double> &result = summer.getResult();

    std::cout << "Первые 10 элементов результата:" << std::endl;
    for (int i = 0; i < 10 && i < L; ++i)
    {
        std::cout << result[i] << " ";
    }
    std::cout << std::endl;

    return 0;
}