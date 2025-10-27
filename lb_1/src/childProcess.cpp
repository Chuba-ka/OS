#include "childProcess.hpp"
#include <iostream>
#include <fstream>
#include <string>

ChildProcess::ChildProcess(int read_fd, const std::string &filename)
    : read_fd_(read_fd), filename_(filename) {}

void ChildProcess::run()
{
    std::ofstream file(filename_);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << filename_ << std::endl;
        close(read_fd_);
        return;
    }

    std::string line;
    char ch;
    while (read(read_fd_, &ch, 1) == 1)
    {
        line += ch;
        if (ch == '\n')
        {
            std::string processed = processor_.removeVowels(line);
            std::cout << processed;
            file << processed;
            line.clear();
        }
    }
    if (!line.empty())
    {
        std::string processed = processor_.removeVowels(line);
        std::cout << processed;
        file << processed;
    }

    file.close();
    close(read_fd_);
}