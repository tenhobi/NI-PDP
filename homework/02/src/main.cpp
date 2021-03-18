#include <iostream>
#include <chrono>

#include "Parser.hpp"
#include "Solver.hpp"

int main(int argc, char **argv) {
    if (argc <= 1) {
        std::cout << "Error: Invalid number of arguments." << std::endl;
        return 1;
    }

    Parser parser;
    ParserOutput parserOutput = parser.parse(argv[1]);
    Solver solver(parserOutput.maxDepth);
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

    return 0;
}
