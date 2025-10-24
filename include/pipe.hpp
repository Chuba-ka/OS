#pragma once

class Pipe
{
public:
    Pipe();
    ~Pipe();
    int getReadFd() const;
    int getWriteFd() const;
    void closeRead();
    void closeWrite();

private:
    int fds_[2];
};
