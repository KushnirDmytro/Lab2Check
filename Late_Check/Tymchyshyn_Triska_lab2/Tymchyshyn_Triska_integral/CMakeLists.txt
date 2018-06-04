cmake_minimum_required(VERSION 3.10)
project(intgral)

set(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pthread -Og -ffast-math")

add_executable(intgral parallel.cpp)