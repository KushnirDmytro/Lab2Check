cmake_minimum_required(VERSION 3.9)
project(integral)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -static-libgcc -static-libstdc++ -O3")
find_package(Threads)

add_executable(integral integral.cpp INIReader.h )
target_link_libraries (integral ${CMAKE_THREAD_LIBS_INIT})