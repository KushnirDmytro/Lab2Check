cmake_minimum_required(VERSION 3.9)
project(Lab2Check)

set(CMAKE_CXX_STANDARD 14)


set(CMAKE_CXX_FLAGS "-O3 -Wall -Wextra -pthread")

add_executable(Lab2Check main.cpp)