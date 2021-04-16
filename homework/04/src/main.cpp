#include <iostream>
#include <chrono>
#include <mpi.h>

#include "Parser.hpp"
#include "Solver.hpp"

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int procNum, procCount;
    MPI_Comm_rank(MPI_COMM_WORLD, &procNum);
    MPI_Comm_size(MPI_COMM_WORLD, &procCount);

    if (argc <= 1) {
        std::cout << "Error: Invalid number of arguments." << std::endl;
        return 1;
    }

    Parser parser;
    ParserOutput parserOutput = parser.parse(argv[1]);
    Solver solver(parserOutput.maxDepth);

    if (procNum != 0) {
        solver.solve(parserOutput.chessboard);

        MPI_Finalize();
        return 0;
    }

    // TIMER START
    auto start = std::chrono::high_resolution_clock::now();

    solver.solve(parserOutput.chessboard);

    auto finish = std::chrono::high_resolution_clock::now();
    // TIMER END

    std::chrono::duration<double> duration = (finish - start);

    std::cout << "Duration: ";
    if (duration.count() <= 10.0) {
        auto durationInMilliSeconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
        std::cout << durationInMilliSeconds << " ms" << std::endl;
    } else {
        auto durationInSeconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
        std::cout << durationInSeconds << " s" << std::endl;
    }

    MPI_Finalize();
    return 0;
}
