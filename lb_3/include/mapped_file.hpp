#pragma once

#include <string>
#include <cstddef>

class MappedFile
{
public:
    MappedFile(const std::string &filename, size_t size);
    ~MappedFile();

    void *getData() const { return data_; }
    size_t getSize() const { return size_; }

    bool isValid() const { return data_ != nullptr && data_ != (void *)-1; }

private:
    int fd_;
    void *data_;
    size_t size_;
};