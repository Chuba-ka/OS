#include "mapped_file.hpp"
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

MappedFile::MappedFile(const std::string &filename, size_t size)
    : fd_(-1), data_((void *)-1), size_(size)
{
    fd_ = open(filename.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (fd_ == -1)
    {
        std::cerr << "Failed to open file for mapping: " << filename << std::endl;
        return;
    }

    if (ftruncate(fd_, size) == -1)
    {
        std::cerr << "Failed to truncate file: " << filename << std::endl;
        close(fd_);
        fd_ = -1;
        return;
    }

    data_ = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0);
    if (data_ == MAP_FAILED)
    {
        std::cerr << "mmap failed for: " << filename << std::endl;
        data_ = nullptr;
        close(fd_);
        fd_ = -1;
    }
}

MappedFile::~MappedFile()
{
    if (data_ != nullptr && data_ != (void *)-1)
    {
        munmap(data_, size_);
    }
    if (fd_ != -1)
    {
        close(fd_);
    }
}