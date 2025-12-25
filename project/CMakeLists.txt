cmake_minimum_required(VERSION 3.10)
project(simple_chat LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

add_executable(server Server.cpp)
add_executable(client Client.cpp)

target_link_libraries(server PRIVATE Threads::Threads rt)
target_link_libraries(client PRIVATE Threads::Threads rt)
