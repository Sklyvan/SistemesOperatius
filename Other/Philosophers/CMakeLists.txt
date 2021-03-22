cmake_minimum_required(VERSION 3.17)
project(Philosophers C)

set(CMAKE_C_STANDARD 99)

find_package(Threads REQUIRED)
SET(CMAKE_CXX_FLAGS -pthread)

add_executable(Philosophers Main.c)

target_link_libraries(${PROJECT_NAME} Threads::Threads)