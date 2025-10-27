#include <iostream>
#include <string>
#include <cstdlib>
#include "childProcess.hpp"

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <read_fd> <filename>" << std::endl;
        return 1;
    }

    int read_fd = std::atoi(argv[1]);
    std::string filename = argv[2];

    ChildProcess child(read_fd, filename);
    child.run();

    return 0;
}