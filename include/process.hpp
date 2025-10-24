#pragma once

class Process
{
public:
    virtual ~Process() = default;
    virtual void run() = 0;
};
