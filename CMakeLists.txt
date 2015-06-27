cmake_minimum_required(VERSION 2.6)

project(tactis)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

file(GLOB_RECURSE SOURCE src/*.c)
add_executable(tactis ${SOURCE})
