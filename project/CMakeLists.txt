cmake_minimum_required(VERSION 3.5)
project(simple_chat LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(server server.cpp)
add_executable(client client.cpp)

find_package(Threads REQUIRED)
target_link_libraries(server PRIVATE Threads::Threads)
target_link_libraries(client PRIVATE Threads::Threads)