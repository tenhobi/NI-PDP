cmake_minimum_required(VERSION 3.16)
project(03)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -Wextra -pedantic -O3 -funroll-loops") # -O3

add_executable(03 src/main.cpp src/Coords.hpp src/Move.hpp src/Parser.hpp src/Chessboard.hpp src/Solver.hpp src/State.hpp)

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()
