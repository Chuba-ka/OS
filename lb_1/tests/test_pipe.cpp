#include <gtest/gtest.h>
#include <unistd.h>
#include "pipe.hpp"

TEST(PipeTest, CreateAndClose)
{
    Pipe pipe;
    EXPECT_GE(pipe.getReadFd(), 0);
    EXPECT_GE(pipe.getWriteFd(), 0);

    const char *test_data = "test";
    char buffer[5] = {0};
    write(pipe.getWriteFd(), test_data, 4);
    read(pipe.getReadFd(), buffer, 4);
    EXPECT_STREQ(buffer, test_data);

    pipe.closeRead();
    pipe.closeWrite();
    EXPECT_EQ(pipe.getReadFd(), -1);
    EXPECT_EQ(pipe.getWriteFd(), -1);
}

TEST(PipeTest, DoubleClose)
{
    Pipe pipe;
    pipe.closeRead();
    pipe.closeRead();
    pipe.closeWrite();
    pipe.closeWrite();
    EXPECT_EQ(pipe.getReadFd(), -1);
    EXPECT_EQ(pipe.getWriteFd(), -1);
}