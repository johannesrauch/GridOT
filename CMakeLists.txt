cmake_minimum_required(VERSION 3.15)

project(ULmon LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_COMPILER /usr/bin/g++)

option(CMAKE_BUILD_TYPE "Build type" Release)

#add_compile_options(-Wall -Werror)
option(ULMON_PROFILE "Flag to turn profiling support on" OFF)
if(ULMON_PROFILE)
    message("ULMON_PROFILE=On")
    add_compile_options(-pg)
    add_link_options(-pg)
endif()

#set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type")

include_directories(
    ${CMAKE_SOURCE_DIR}/.
)

find_library(LEMON libemon.a)

option(ULMON_COMPILE_TESTS "Flag to compile tests" ON)
add_subdirectory(test)

option(ULMON_COMPILE_BENCHMARK "Flag to compile benchmarks" OFF)
if(ULMON_COMPILE_BENCHMARK)
    add_subdirectory(benchmark)
endif()
