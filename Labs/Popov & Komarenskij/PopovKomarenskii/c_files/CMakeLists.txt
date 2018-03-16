cmake_minimum_required(VERSION 3.7)
project(c_files)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES integrate.cpp)

add_executable(c_files integrate.cpp)
