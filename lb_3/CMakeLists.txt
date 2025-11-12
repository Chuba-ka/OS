# CMakeLists.txt
cmake_minimum_required(VERSION 3.14)
project(ProcessFileMapping LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(include)

# -------------------------------
# Исполняемые файлы
# -------------------------------
add_executable(parent
    src/parent.cpp
    src/mapped_file.cpp
)

add_executable(child
    src/child.cpp
    src/childProcess.cpp
    src/childProcessor.cpp
    src/mapped_file.cpp
)

# -------------------------------
# Google Test
# -------------------------------
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(tests
    tests/test_removeVowels.cpp
    src/childProcessor.cpp
)

target_include_directories(tests PRIVATE include)
target_link_libraries(tests GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(tests)