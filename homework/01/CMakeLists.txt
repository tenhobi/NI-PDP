cmake_minimum_required(VERSION 3.16)
project(01)

set(CMAKE_CXX_STANDARD 17)

add_executable(01 src/main.cpp src/Coords.hpp src/Move.hpp src/Parser.hpp src/Chessboard.hpp src/Solver.hpp)
