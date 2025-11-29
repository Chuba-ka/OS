#include <iostream>
#include <dlfcn.h>
#include <vector>
#include <sstream>
#include "../include/PiCalculator.hpp"
#include "../include/ECalculator.hpp"

typedef PiCalculator *(*create_pi_t)();
typedef ECalculator *(*create_e_t)();
typedef void (*destroy_pi_t)(PiCalculator *);
typedef void (*destroy_e_t)(ECalculator *);

struct Libs
{
    void *handle;
    PiCalculator *pi;
    ECalculator *e;
    const char *path;
    const char *desc;
};

Libs load(const char *path, const char *desc)
{
    void *h = dlopen(path, RTLD_LAZY);
    if (!h)
    {
        std::cerr << "Ошибка загрузки библиотеки: " << path << ": " << dlerror() << "\n";
        exit(1);
    }

    auto cp = (create_pi_t)dlsym(h, "create_pi");
    auto ce = (create_e_t)dlsym(h, "create_e");

    return {h, cp(), ce(), path, desc};
}

void unload(Libs &l)
{
    auto dp = (destroy_pi_t)dlsym(l.handle, "destroy_pi");
    auto de = (destroy_e_t)dlsym(l.handle, "destroy_e");
    dp(l.pi);
    de(l.e);
    dlclose(l.handle);
}

int main()
{
    const char *libs[2] = {"./lib1.so", "./lib2.so"};
    const char *names[2] = {"Ряд Лейбница + предел (1+1/x)^x", "Формула Валлиса + ряд Тейлора"};

    int cur = 0;
    Libs current = load(libs[cur], names[cur]);

    std::cout << "Текущая реализация: " << current.desc << "\n";
    std::cout << "Команда 0 — переключить реализацию\n";

    std::string line;
    while (std::cout << "\n> ", std::getline(std::cin, line))
    {
        if (line.empty())
            continue;

        std::istringstream iss(line);
        int cmd;
        if (!(iss >> cmd))
        {
            std::cout << "Введите команду\n";
            continue;
        }

        if (cmd == 0)
        {
            unload(current);
            cur = 1 - cur;
            current = load(libs[cur], names[cur]);
            std::cout << "Переключено на: " << current.desc << "\n";
            continue;
        }

        int arg;
        if (!(iss >> arg))
        {
            std::cout << "Требуется хотя бы один аргумент\n";
            continue;
        }

        if (cmd == 1)
        {
            if (arg <= 0)
            {
                std::cout << "K должно быть > 0\n";
                continue;
            }
            std::cout << "π (" << current.pi->name() << ", K=" << arg << ") = "
                      << current.pi->calculate(arg) << "\n";
        }
        else if (cmd == 2)
        {
            if (arg < 0)
            {
                std::cout << "x должно быть ≥ 0\n";
                continue;
            }
            std::cout << "e (" << current.e->name() << ", x=" << arg << ") = "
                      << current.e->calculate(arg) << "\n";
        }
        else
        {
            std::cout << "Доступные команды: 1 K, 2 x, 0 — смена\n";
        }
    }

    unload(current);
    return 0;
}