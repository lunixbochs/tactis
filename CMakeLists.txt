cmake_minimum_required(VERSION 2.6)

project(tactis)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
include_directories(src/tactis)

file(GLOB_RECURSE TACTIS_SOURCE src/tactis/*.c)
add_library(tactis STATIC ${TACTIS_SOURCE})

file(GLOB_RECURSE REPL_SOURCE src/repl/*.c)
add_executable(repl ${REPL_SOURCE})
target_link_libraries(repl tactis)
