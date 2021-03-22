cmake_minimum_required(VERSION 3.17)
project(Code C)

set(CMAKE_C_STANDARD 99)

find_package(Threads REQUIRED)

SET(CMAKE_CXX_FLAGS -pthread)

add_executable(Code main.c)

target_link_libraries(${PROJECT_NAME} Threads::Threads)