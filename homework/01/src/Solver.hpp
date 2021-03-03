#ifndef INC_01_SOLVER_HPP
#define INC_01_SOLVER_HPP

#include <algorithm>

#include "Chessboard.hpp"
#include "Move.hpp"

class Solver {
public:
    Solver(unsigned long maxDepth) : maxDepth(maxDepth) {}

    void solve(const Chessboard &chessboard) {
        std::cout << chessboard;
        std::cout << " --- --- --- " << std::endl;
        recursion(0, true, chessboard, std::vector<int>());

        std::cout << "WIN in steps: " << bestPrice << std::endl;
        std::cout << "CALLS: " << calls << std::endl;
        std::cout << "RET1: " << ret1 << std::endl;
        std::cout << "RET2: " << ret2 << std::endl;
        std::cout << "RET3: " << ret3 << std::endl;
        std::cout << "RET4: " << ret4 << std::endl;
        std::cout << "MOVES COUNT: " << movesCount << std::endl;

        int turnIndex = 1;
        for (int move: bestTurnMoves) {
            std::cout << (turnIndex % 2 == 1 ? 'S' : 'J') << " " << chessboard.indexToCoords(move) << std::endl;
            turnIndex++;
        }

        std::cout << std::endl;
    }

private:
    unsigned long maxDepth;
    unsigned long calls = 0;

    unsigned long movesCount = 0;
    unsigned long ret1 = 0;
    unsigned long ret2 = 0;
    unsigned long ret3 = 0;
    unsigned long ret4 = 0;

    bool solutionFound = false;
    unsigned long bestPrice = 0;
    std::vector<int> bestTurnMoves;

    void recursion(unsigned long depth, bool bishopTurn, const Chessboard &chessboard, const std::vector<int> &turnMoves) {
        if (chessboard.enemiesCount <= 0) {
            if (!solutionFound) {
                solutionFound = true;
                bestPrice = depth;
                bestTurnMoves = turnMoves;

                std::cout << " FIRST SOLUTION " << std::endl;
            } else if (bestPrice > depth) {
                bestPrice = depth;
                bestTurnMoves = turnMoves;

                std::cout << " NEW SOLUTION " << std::endl;
            }

            ret1++;
            return;
        }

        if (depth >= maxDepth) {
            ret2++;
            return;
        }

        if (depth + chessboard.enemiesCount >= maxDepth) {
            ret3++;
            return;
        }

        if (solutionFound) {
            if (depth + chessboard.enemiesCount >= bestPrice) {
                ret4++;
                return;
            }
        }

        calls++;

        std::vector<Move> moves = chessboard.getAllMoves(bishopTurn);
        std::sort(moves.begin(), moves.end());

        movesCount += moves.size();

        for (auto &move: moves) {
            Chessboard chessboardForMove = chessboard.clone();
            chessboardForMove.moveToSquare(bishopTurn, move.coords);

            std::vector<int> turnMovesForMove = turnMoves;
            turnMovesForMove.push_back(Chessboard::coordsToIndex(chessboard.size, move.coords));

            recursion(depth + 1, !bishopTurn, chessboardForMove, turnMovesForMove);
        }
    }
};

#endif //INC_01_SOLVER_HPP
