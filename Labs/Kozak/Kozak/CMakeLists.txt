cmake_minimum_required(VERSION 3.9)
project(threads)

set(CMAKE_CXX_STANDARD 11)


set(CMAKE_CXX_FLAGS "-O3 -Wall -Wextra -pthread")

add_executable(threads main.cpp)
