#include "mapped_file.hpp"
#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>

int main()
{
    std::string file1, file2;
    std::getline(std::cin, file1);
    std::getline(std::cin, file2);

    const size_t BUFFER_SIZE = 4096;

    MappedFile map1(file1, BUFFER_SIZE);
    MappedFile map2(file2, BUFFER_SIZE);

    if (!map1.isValid() || !map2.isValid())
    {
        std::cerr << "Failed to create mapped files" << std::endl;
        return 1;
    }

    pid_t pid1 = fork();
    if (pid1 == 0)
    {
        execl("./bin/child", "child", file1.c_str(), nullptr);
        std::cerr << "Exec failed for child1" << std::endl;
        _exit(1);
    }
    else if (pid1 < 0)
    {
        std::cerr << "Fork failed for child1" << std::endl;
        return 1;
    }

    pid_t pid2 = fork();
    if (pid2 == 0)
    {
        execl("./bin/child", "child", file2.c_str(), nullptr);
        std::cerr << "Exec failed for child2" << std::endl;
        _exit(1);
    }
    else if (pid2 < 0)
    {
        std::cerr << "Fork failed for child2" << std::endl;
        return 1;
    }

    sleep(1);

    char *buf1 = static_cast<char *>(map1.getData());
    char *buf2 = static_cast<char *>(map2.getData());

    std::string line;
    while (std::getline(std::cin, line))
    {
        line += '\n';
        char *target = (line.length() - 1 <= 10) ? buf1 : buf2;

        while (target[0] != '\0')
        {
            usleep(1000);
        }

        std::strncpy(target, line.c_str(), BUFFER_SIZE - 1);
        target[BUFFER_SIZE - 1] = '\0';
    }

    std::memset(buf1, 0, BUFFER_SIZE);
    std::memset(buf2, 0, BUFFER_SIZE);

    waitpid(pid1, nullptr, 0);
    waitpid(pid2, nullptr, 0);

    return 0;
}