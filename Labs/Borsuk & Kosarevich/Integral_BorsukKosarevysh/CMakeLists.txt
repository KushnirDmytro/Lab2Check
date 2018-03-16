cmake_minimum_required(VERSION 3.9)
project(integral)

set(CMAKE_CXX_STANDARD 11)

find_package(Threads)

add_executable(simple_integral simple_integral.cpp getconfig.h getconfig.cpp)
add_executable(integral integral.cpp getconfig.h getconfig.cpp)
target_link_libraries (integral ${CMAKE_THREAD_LIBS_INIT})