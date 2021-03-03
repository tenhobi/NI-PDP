#ifndef INC_01_SOLVER_HPP
#define INC_01_SOLVER_HPP

#include <algorithm>

#include "Chessboard.hpp"
#include "Move.hpp"

class Solver {
public:
    Solver(unsigned long maxDepth) : maxDepth(maxDepth) {}

    void solve(const Chessboard &chessboard) {
        recursion(0, chessboard, std::vector<int>());

        std::cout << "Price: " << bestPrice << std::endl;
        std::cout << "Number of calls: " << calls << std::endl;
        std::cout << "Turns:" << std::endl;

        int turnIndex = 1;
        for (int move: bestTurnMoves) {
            Coords moveCoords = chessboard.indexToCoords(move);
            std::cout << (turnIndex % 2 == 1 ? 'S' : 'J') << " -> " << moveCoords << (chessboard.getSquare(moveCoords) ? '*' : ' ') << std::endl;
            turnIndex++;
        }
    }

private:
    unsigned long maxDepth;
    unsigned long calls = 0;

    bool solutionFound = false;
    unsigned long bestPrice = 0;
    std::vector<int> bestTurnMoves;

    void recursion(unsigned long depth, const Chessboard &chessboard, const std::vector<int> &turnMoves) {
        calls++;

        if (chessboard.enemiesCount <= 0) {
            if (!solutionFound) {
                solutionFound = true;
                bestPrice = depth;
                bestTurnMoves = turnMoves;
            } else if (bestPrice > depth) {
                bestPrice = depth;
                bestTurnMoves = turnMoves;
            }
            return;
        }

        if (depth >= maxDepth) {
            return;
        }

        if (depth + chessboard.enemiesCount >= maxDepth) {
            return;
        }

        if (solutionFound) {
            if (depth + chessboard.enemiesCount >= bestPrice) {
                return;
            }
        }

        std::vector<Move> moves = chessboard.getAllMoves(depth % 2 == 0);
        std::sort(moves.begin(), moves.end());

        for (auto &move: moves) {
            Chessboard chessboardForMove = chessboard.clone();
            chessboardForMove.moveToSquare(depth % 2 == 0, move.coords);

            std::vector<int> turnMovesForMove = turnMoves;
            turnMovesForMove.emplace_back(Chessboard::coordsToIndex(chessboard.size, move.coords));

            recursion(depth + 1, chessboardForMove, turnMovesForMove);
        }
    }
};

#endif //INC_01_SOLVER_HPP
