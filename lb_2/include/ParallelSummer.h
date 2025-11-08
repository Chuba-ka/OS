#pragma once

#include <vector>
#include <pthread.h>
#include <semaphore.h>

class ParallelSummer
{
private:
    const std::vector<std::vector<double>> &arrays;
    int length;
    int max_threads;
    std::vector<double> result;
    pthread_mutex_t mutex;
    sem_t thread_sem;

    struct ThreadArg
    {
        int start;
        int end;
        ParallelSummer *self;
    };

    static void *sumRange(void *arg);

public:
    ParallelSummer(const std::vector<std::vector<double>> &arrs, int len, int max_th);
    ~ParallelSummer();
    void sumArrays();
    const std::vector<double> &getResult() const;
};
