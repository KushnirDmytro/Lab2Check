cmake_minimum_required(VERSION 3.9)
project(IntegralComputing)

find_package (Threads)
find_package(Boost COMPONENTS program_options REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})

set(CMAKE_CXX_STANDARD 14)

add_executable(ParallelIntegralComputing parallel.cpp)
add_executable(LinearIntegralComputing linear.cpp)

target_link_libraries(ParallelIntegralComputing ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(ParallelIntegralComputing  ${Boost_LIBRARIES})
target_link_libraries(LinearIntegralComputing ${Boost_LIBRARIES})
