cmake_minimum_required(VERSION 3.19)
project(ThreadsSystemVSemaphores C)

set(CMAKE_C_STANDARD 99)

add_executable(ThreadsSystemVSemaphores main.c)

target_link_libraries(ThreadsSystemVSemaphores -lpthread)