cmake_minimum_required(VERSION 3.10)
project(ParallelArraySummer LANGUAGES CXX)

# Устанавливаем стандарт C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Включаем поддержку потоков (pthread)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Добавляем исполняемый файл
add_executable(${PROJECT_NAME}
    src/main.cpp
    src/ArrayManager.cpp
    src/ParallelSummer.cpp
)

# Указываем директорию с заголовочными файлами
target_include_directories(${PROJECT_NAME} PRIVATE include)

# Линкуем с pthread
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)

# Опционально: включаем отладочную информацию и предупреждения
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -g)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -O3)
endif()