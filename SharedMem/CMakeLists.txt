cmake_minimum_required(VERSION 3.19)
project(SharedMem C)

set(CMAKE_C_STANDARD 99)

add_executable(SharedMem main1.c main2.c main1.c mem.h)

target_link_libraries(SharedMem -lrt)