#include "childProcess.hpp"
#include "mapped_file.hpp"
#include <iostream>
#include <fstream>
#include <cstring>
#include <unistd.h>

ChildProcess::ChildProcess(const std::string &mapped_filename)
    : mapped_filename_(mapped_filename) {}

void ChildProcess::run()
{
    const size_t BUFFER_SIZE = 4096;
    MappedFile mapped_file(mapped_filename_, BUFFER_SIZE);
    if (!mapped_file.isValid())
    {
        std::cerr << "Child failed to map file: " << mapped_filename_ << std::endl;
        return;
    }

    std::ofstream output_file(mapped_filename_ + ".out");
    if (!output_file.is_open())
    {
        std::cerr << "Failed to open output file: " << mapped_filename_ << ".out" << std::endl;
        return;
    }

    char *buffer = static_cast<char *>(mapped_file.getData());
    size_t pos = 0;

    while (true)
    {
        if (buffer[pos] == '\0' && pos > 0)
        {
            std::string line(buffer, pos);
            std::string processed = processor_.removeVowels(line);
            std::cout << processed;
            output_file << processed;
            output_file.flush();

            std::memset(buffer, 0, BUFFER_SIZE);
            pos = 0;
        }
        else if (buffer[pos] != '\0')
        {
            if (++pos >= BUFFER_SIZE - 1)
            {
                std::cerr << "Buffer overflow in child" << std::endl;
                break;
            }
        }
        usleep(1000);
    }
}