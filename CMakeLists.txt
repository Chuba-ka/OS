cmake_minimum_required(VERSION 3.10)
project(OSProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(include)

add_executable(parent src/parent.cpp src/pipe.cpp)
add_executable(child src/child.cpp src/childProcess.cpp src/childProcessor.cpp)