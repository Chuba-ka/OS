#include "childProcess.hpp"
#include <iostream>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <mapped_filename>" << std::endl;
        return 1;
    }

    ChildProcess child(argv[1]);
    child.run();
    return 0;
}