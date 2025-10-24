#pragma once

#include "process.hpp"
#include "childProcessor.hpp"
#include <string>
#include <unistd.h>

class ChildProcess : public Process
{
public:
    ChildProcess(int read_fd, const std::string &filename);
    void run() override;

private:
    int read_fd_;
    std::string filename_;
    ChildProcessor processor_;
};
