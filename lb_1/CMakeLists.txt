cmake_minimum_required(VERSION 3.10)
project(VowelRemover LANGUAGES CXX)

# Стандарт C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Путь к заголовочным файлам
include_directories(${CMAKE_SOURCE_DIR}/include)

# === Родительский процесс ===
add_executable(parent
    src/parent.cpp
    src/pipe.cpp
)

# === Дочерний процесс ===
add_executable(child
    src/child.cpp
    src/childProcess.cpp
    src/childProcessor.cpp
    src/pipe.cpp
)

# Помещаем оба бинарника в корень сборки
set_target_properties(parent child PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)