cmake_minimum_required(VERSION 3.5)
project(INTGRL)

find_package (Threads)
set(CMAKE_CXX_STANDARD 14)


set(CMAKE_CXX_FLAGS "-Og -Wall -Wextra -pthread -lboost_system -lboost_filesystem -lboost_system")


add_executable(INTGRL main.cpp)
target_link_libraries (INTGRL ${CMAKE_THREAD_LIBS_INIT})
