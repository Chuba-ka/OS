#include "pipe.hpp"
#include <unistd.h>
#include <iostream>

Pipe::Pipe()
{
    fds_[0] = -1;
    fds_[1] = -1;
    if (pipe(fds_) == -1)
    {
        std::cerr << "Pipe creation failed" << std::endl;
    }
}

Pipe::~Pipe()
{
    closeRead();
    closeWrite();
}

int Pipe::getReadFd() const
{
    return fds_[0];
}

int Pipe::getWriteFd() const
{
    return fds_[1];
}

void Pipe::closeRead()
{
    if (fds_[0] != -1)
    {
        close(fds_[0]);
        fds_[0] = -1;
    }
}

void Pipe::closeWrite()
{
    if (fds_[1] != -1)
    {
        close(fds_[1]);
        fds_[1] = -1;
    }
}