cmake_minimum_required(VERSION 3.16.3)
project(04)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -Wextra -pedantic -O3") # -O3

add_executable(04 src/main.cpp src/Coords.hpp src/Move.hpp src/Parser.hpp src/Chessboard.hpp src/Solver.hpp src/State.hpp)

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(04 PUBLIC OpenMP::OpenMP_CXX)
endif()

find_package(MPI REQUIRED)
message(STATUS "Run: ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} ${MPIEXEC_MAX_NUMPROCS} ${MPIEXEC_PREFLAGS} EXECUTABLE ${MPIEXEC_POSTFLAGS} ARGS")
target_link_libraries(04 PUBLIC MPI::MPI_CXX)

