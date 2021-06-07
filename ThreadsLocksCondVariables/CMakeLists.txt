cmake_minimum_required(VERSION 3.19)
project(ThreadsLocksCondVariables C)

set(CMAKE_C_STANDARD 99)

add_executable(ThreadsLocksCondVariables main.c)

target_link_libraries(ThreadsLocksCondVariables -lpthread)