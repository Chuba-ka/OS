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

# === Цели для трассировки системных вызовов ===

# Трассировка только родительского процесса
add_custom_target(
    trace_parent
    COMMAND strace -f -o ${CMAKE_SOURCE_DIR}/syscalls_parent.log ./parent
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    DEPENDS parent
    COMMENT "Running parent with strace to trace system calls"
)

# Трассировка с временными метками (более детальная)
add_custom_target(
    trace_parent_detailed
    COMMAND strace -f -tt -o ${CMAKE_SOURCE_DIR}/syscalls_parent_detailed.log ./parent
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    DEPENDS parent
    COMMENT "Running parent with detailed strace (with timestamps)"
)

# Трассировка с ограничением по длине строки (чтобы не обрезались длинные вызовы)
add_custom_target(
    trace_parent_full
    COMMAND strace -f -s 1000 -o ${CMAKE_SOURCE_DIR}/syscalls_parent_full.log ./parent
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    DEPENDS parent
    COMMENT "Running parent with full strace (long strings not truncated)"
)

# Проверка наличия strace при конфигурации
find_program(STRACE_EXECUTABLE strace)
if(NOT STRACE_EXECUTABLE)
    message(WARNING "strace not found! Trace targets will not work.")
endif()