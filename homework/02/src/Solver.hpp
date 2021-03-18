#ifndef INC_01_SOLVER_HPP
#define INC_01_SOLVER_HPP

#include <algorithm>

#include "Chessboard.hpp"
#include "Move.hpp"

class Solver {
public:
    explicit Solver(unsigned long maxDepth) : maxDepth(maxDepth) {}

    void solve(Chessboard &chessboard) {
        #pragma omp parallel firstprivate(chessboard) default(none)
        {
            #pragma omp single
            {
                std::vector<int> moves;
                recursion(0, chessboard, moves);
            }
        }

        std::cout << "Price: " << bestPrice << std::endl;
        std::cout << "Number of calls: " << calls << std::endl;
        std::cout << "Turns:" << std::endl;

        int turnIndex = 1;
        for (int move: bestTurnMoves) {
            Coords moveCoords = chessboard.indexToCoords(move);
            std::cout << (turnIndex % 2 == 1 ? 'S' : 'J') << " -> " << moveCoords
                      << (chessboard.getSquare(moveCoords) ? '*' : ' ') << std::endl;
            turnIndex++;
        }
    }

private:
    unsigned long maxDepth;
    unsigned long calls = 0;

    unsigned long bestPrice = std::numeric_limits<unsigned long>::max();
    std::vector<int> bestTurnMoves;

    void recursion(unsigned long depth, Chessboard &chessboard, std::vector<int> &turnMoves) {
        if (depth >= 0.35 * maxDepth) {
            recursionSeq(depth, chessboard, turnMoves);
            return;
        }

        calls++;

        if (shouldExit(depth, chessboard, turnMoves)) {
            return;
        }

        std::vector<Move> moves = chessboard.getAllMoves(depth % 2 == 0);
        std::sort(moves.begin(), moves.end());

        for (auto &move: moves) {
            #pragma omp task firstprivate(chessboard, move, depth, turnMoves) default(none)
            {
                Chessboard chessboardForMove = chessboard.clone();
                chessboardForMove.moveToSquare(depth % 2 == 0, move.coords);

                std::vector<int> turnMovesForMove = turnMoves;
                turnMovesForMove.emplace_back(Chessboard::coordsToIndex(chessboard.size, move.coords));

                recursion(depth + 1, chessboardForMove, turnMovesForMove);
            }
        }
    }

    void recursionSeq(unsigned long depth, Chessboard &chessboard, std::vector<int> &turnMoves) {
        calls++;

        if (shouldExit(depth, chessboard, turnMoves)) {
            return;
        }

        std::vector<Move> moves = chessboard.getAllMoves(depth % 2 == 0);
        std::sort(moves.begin(), moves.end());

        for (auto &move: moves) {
            Coords currentPosition = chessboard.currentPlayerPosition(depth % 2 == 0);
            bool currentMoveValue = chessboard.getSquare(move.coords);

            chessboard.moveToSquare(depth % 2 == 0, move.coords);
            turnMoves.emplace_back(Chessboard::coordsToIndex(chessboard.size, move.coords));

            recursionSeq(depth + 1, chessboard, turnMoves);

            turnMoves.pop_back();
            chessboard.moveToSquare(depth % 2 == 0, currentPosition);
            chessboard.setSquare(move.coords, currentMoveValue);
            if (currentMoveValue) {
                chessboard.enemiesCount++;
            }
        }
    }

    bool shouldExit(unsigned long depth, const Chessboard &chessboard, const std::vector<int> &turnMoves) {
        if (chessboard.enemiesCount <= 0) {
            if (bestPrice > depth) {
                # pragma omp critical
                if (bestPrice > depth) {
                    bestPrice = depth;
                    bestTurnMoves = turnMoves;
                }
            }
            return true;
        }

        if (depth >= maxDepth) {
            return true;
        }

        if (depth + chessboard.enemiesCount >= maxDepth) {
            return true;
        }

        if (depth + chessboard.enemiesCount >= bestPrice) {
            return true;
        }

        return false;
    }
};

#endif //INC_01_SOLVER_HPP
