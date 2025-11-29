cmake_minimum_required(VERSION 3.16)
project(PiE_Lab_OOP LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(lib1 SHARED
    lib1_leibniz_limit/PiLeibniz.cpp
    lib1_leibniz_limit/ELimit.cpp
)
target_include_directories(lib1 PUBLIC include)

add_library(lib2 SHARED
    lib2_wallis_series/PiWallis.cpp
    lib2_wallis_series/ESeries.cpp
)
target_include_directories(lib2 PUBLIC include)

add_executable(static_app static_app/main_static.cpp)
target_link_libraries(static_app lib2)        # или lib1 — как хочешь
target_include_directories(static_app PRIVATE include)

add_executable(dynamic_app dynamic_app/main_dynamic.cpp)
target_link_libraries(dynamic_app dl)
target_include_directories(dynamic_app PRIVATE include)

# Чтобы библиотеки лежали в build/
set_target_properties(lib1 lib2 PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}  
)

add_custom_command(TARGET lib1 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:lib1> ${CMAKE_BINARY_DIR}/lib1.so)
add_custom_command(TARGET lib2 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:lib2> ${CMAKE_BINARY_DIR}/lib2.so)