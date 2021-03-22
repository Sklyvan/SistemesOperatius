cmake_minimum_required(VERSION 3.17)
project(CurrentCode C)

set(CMAKE_C_STANDARD 99)

find_package(Threads REQUIRED)
SET(CMAKE_CXX_FLAGS -pthread)

include_directories(.)

add_executable(CurrentCode main.c)

target_link_libraries(${PROJECT_NAME} Threads::Threads)