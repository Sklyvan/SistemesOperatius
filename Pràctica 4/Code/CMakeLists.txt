cmake_minimum_required(VERSION 3.17)
project(Code C)

set(CMAKE_C_STANDARD 11)

find_package(Threads REQUIRED)
SET(CMAKE_CXX_FLAGS -pthread)

include_directories(.)

add_executable(Code myutils.c myutils.h Bank_Transfer.c)

target_link_libraries(${PROJECT_NAME} Threads::Threads)