#include <iostream>
#include <sstream>
#include "../include/PiCalculator.hpp"
#include "../include/ECalculator.hpp"

extern "C" PiCalculator *create_pi();
extern "C" ECalculator *create_e();
extern "C" void destroy_pi(PiCalculator *);
extern "C" void destroy_e(ECalculator *);

int main()
{
    PiCalculator *pi = create_pi();
    ECalculator *e = create_e();

    std::cout << "Статическая линковка\n";
    std::cout << "Используется: " << pi->name() << " + " << e->name() << "\n";
    std::cout << "Для выхода введите 0\n";

    std::string line;
    while (std::cout << "\n> ", std::getline(std::cin, line))
    {
        if (line.empty())
            continue;

        std::istringstream iss(line);
        int cmd;
        if (!(iss >> cmd))
            continue;

        if (cmd == 0)
            break;

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
            std::cout << "π (" << pi->name() << ", K=" << arg << ") = "
                      << pi->calculate(arg) << "\n";
        }
        else if (cmd == 2)
        {
            if (arg < 0)
            {
                std::cout << "x должно быть ≥ 0\n";
                continue;
            }
            std::cout << "e (" << e->name() << ", x=" << arg << ") = "
                      << e->calculate(arg) << "\n";
        }
        else
        {
            std::cout << "Доступные команды: 1 K, 2 x, 0 — выход\n";
        }
    }

    destroy_pi(pi);
    destroy_e(e);
    return 0;
}