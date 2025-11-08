// main.cpp — Автоматический бенчмарк с записью в CSV
#include <iostream>
#include <fstream>
#include <vector>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <chrono>
#include <sstream>
#include "ArrayManager.h"
#include "ParallelSummer.h"

using namespace std::chrono;

int main(int argc, char *argv[])
{
    int min_threads = 1;
    int max_threads = 16;
    int step = 1;
    int num_runs = 10;
    std::string output_file = "results.csv";

    if (argc >= 2)
        max_threads = std::atoi(argv[1]);
    if (argc >= 3)
        num_runs = std::atoi(argv[2]);
    if (argc >= 4)
        output_file = argv[3];

    if (max_threads < 1 || num_runs < 1)
    {
        std::cerr << "Ошибка: параметры должны быть положительными.\n";
        return 1;
    }

    std::srand(std::time(nullptr));
    const int K = 10;
    const int L = 1000000;

    std::ofstream csv(output_file);
    if (!csv.is_open())
    {
        std::cerr << "Не удалось открыть файл: " << output_file << std::endl;
        return 1;
    }

    csv << "threads,run,time_ms\n";

    std::cout << "Запуск бенчмарка: потоки от " << min_threads
              << " до " << max_threads << ", запусков: " << num_runs << std::endl;

    ArrayManager array_manager(K, L);
    array_manager.fillRandom(0.0, 100.0);
    const auto &arrays = array_manager.getArrays();

    for (int threads = min_threads; threads <= max_threads; threads += step)
    {
        double total_time = 0.0;

        for (int run = 1; run <= num_runs; ++run)
        {
            ParallelSummer summer(arrays, L, threads);

            auto start = high_resolution_clock::now();
            summer.sumArrays();
            auto end = high_resolution_clock::now();

            double time_ms = duration<double, std::milli>(end - start).count();
            total_time += time_ms;

            csv << threads << "," << run << "," << std::fixed << std::setprecision(3) << time_ms << "\n";

            std::cout << "Потоков: " << std::setw(2) << threads
                      << " | Запуск: " << run << "/" << num_runs
                      << " | Время: " << std::setw(8) << time_ms << " мс\r";
            std::cout.flush();
        }

        double avg = total_time / num_runs;
        std::cout << "\n  → Среднее для " << threads << " потоков: " << avg << " мс\n";
    }

    csv.close();
    std::cout << "\nГотово! Данные сохранены в: " << output_file << std::endl;
    std::cout << "Теперь запустите: python3 plot.py\n";

    return 0;
}