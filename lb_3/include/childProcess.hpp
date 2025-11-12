#pragma once

#include "process.hpp"
#include "childProcessor.hpp"
#include <string>

class ChildProcess : public Process
{
public:
    ChildProcess(const std::string &mapped_filename);
    void run() override;

private:
    std::string mapped_filename_;
    ChildProcessor processor_;
};