cmake_minimum_required(VERSION 3.8)
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "-O3 -Wall -Wextra")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
set(SOURCE_FILES main.cpp)
file(APPEND "config.txt" "")
file(WRITE "config.txt" "4<-thread number\n0.001<-absolute error\n0.001<-relative error\n-50<-min x\n50<-max x\n-50<-min y\n50<-max y\n200<-init iterations\n16000<-max iterations ")
add_executable(paralel_integrals ${SOURCE_FILES})
