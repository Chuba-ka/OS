#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
#include "pipe.hpp"

int main()
{
    std::string file1, file2;
    std::getline(std::cin, file1);
    std::getline(std::cin, file2);

    Pipe pipe1;
    Pipe pipe2;

    pid_t pid1 = fork();
    if (pid1 == 0)
    {
        pipe1.closeWrite();
        pipe2.closeRead();
        pipe2.closeWrite();
        execl("./child", "./child", std::to_string(pipe1.getReadFd()).c_str(), file1.c_str(), nullptr);
        std::cerr << "Exec failed for child1" << std::endl;
        exit(1);
    }
    else if (pid1 < 0)
    {
        std::cerr << "Fork failed for child1" << std::endl;
        return 1;
    }

    pid_t pid2 = fork();
    if (pid2 == 0)
    {
        pipe2.closeWrite();
        pipe1.closeRead();
        pipe1.closeWrite();
        execl("./child", "./child", std::to_string(pipe2.getReadFd()).c_str(), file2.c_str(), nullptr);
        std::cerr << "Exec failed for child2" << std::endl;
        exit(1);
    }
    else if (pid2 < 0)
    {
        std::cerr << "Fork failed for child2" << std::endl;
        return 1;
    }

    pipe1.closeRead();
    pipe2.closeRead();

    std::string line;
    while (std::getline(std::cin, line))
    {
        line += "\n";
        if (line.length() - 1 > 10)
        {
            write(pipe2.getWriteFd(), line.c_str(), line.length());
        }
        else
        {
            write(pipe1.getWriteFd(), line.c_str(), line.length());
        }
    }

    pipe1.closeWrite();
    pipe2.closeWrite();

    waitpid(pid1, nullptr, 0);
    waitpid(pid2, nullptr, 0);

    return 0;
}