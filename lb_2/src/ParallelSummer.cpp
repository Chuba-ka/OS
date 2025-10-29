// ParallelSummer.cpp
#include "ParallelSummer.h"
#include <iostream>
#include <algorithm>

ParallelSummer::ParallelSummer(const std::vector<std::vector<double>> &arrs, int len, int max_th)
    : arrays(arrs), length(len), max_threads(max_th), result(len, 0.0)
{
    pthread_mutex_init(&mutex, nullptr);
    sem_init(&thread_sem, 0, max_threads);
}

ParallelSummer::~ParallelSummer()
{
    pthread_mutex_destroy(&mutex);
    sem_destroy(&thread_sem);
}

void *ParallelSummer::sumRange(void *arg)
{
    ThreadArg *t_arg = static_cast<ThreadArg *>(arg);
    ParallelSummer *self = t_arg->self;
    int start = t_arg->start;
    int end = t_arg->end;

    std::vector<double> local_sums(end - start, 0.0);
    int num_arrays = self->arrays.size();

    for (int idx = start; idx < end; ++idx)
    {
        for (int k = 0; k < num_arrays; ++k)
        {
            local_sums[idx - start] += self->arrays[k][idx];
        }
    }

    pthread_mutex_lock(&self->mutex);
    for (int idx = start; idx < end; ++idx)
    {
        self->result[idx] += local_sums[idx - start];
    }
    pthread_mutex_unlock(&self->mutex);

    sem_post(&self->thread_sem);
    delete t_arg;
    return nullptr;
}

void ParallelSummer::sumArrays()
{
    int num_threads = std::min(max_threads, length);
    int min_chunk = 100;
    num_threads = std::min(num_threads, length / min_chunk + 1);
    if (num_threads <= 0)
        num_threads = 1;

    std::vector<pthread_t> threads(num_threads);
    int chunk_size = length / num_threads;
    int remainder = length % num_threads;

    int current_start = 0;
    for (int i = 0; i < num_threads; ++i)
    {
        int current_end = current_start + chunk_size + (i < remainder ? 1 : 0);
        sem_wait(&thread_sem);

        ThreadArg *arg = new ThreadArg{current_start, current_end, this};

        if (pthread_create(&threads[i], nullptr, sumRange, arg) != 0)
        {
            std::cerr << "Ошибка создания потока " << i << std::endl;
            sem_post(&thread_sem);
            delete arg;
        }

        current_start = current_end;
    }

    for (int i = 0; i < num_threads; ++i)
    {
        pthread_join(threads[i], nullptr);
    }
}

const std::vector<double> &ParallelSummer::getResult() const
{
    return result;
}