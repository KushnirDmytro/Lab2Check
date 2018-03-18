cmake_minimum_required(VERSION 3.7)
project(c_files)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES integrate.cpp)

set(CMAKE_CXX_FLAGS "-O3 -Wall -Wextra -pthread")

add_executable(c_files integrate.cpp)
