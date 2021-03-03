#ifndef INC_01_SOLVER_HPP
#define INC_01_SOLVER_HPP

#include <algorithm>
#include "Chessboard.hpp"
#include "Move.hpp"

class Solver {
public:
    void solve(const Chessboard &chessboard) {
        std::cout << "lmao" << std::endl;
        recursion(0, true, chessboard);
    }

    void printResult() {

    }

private:
    bool shouldExit = false;

    // TODO: dolní mez -> q
    // TODO: horní mez -> jezdec k^2, střelec --
    // TODO: prořezávání -> akt_hloubka + (q - #_uz_sebranych_figurek) < akt_min

    // TODO: akt_min -> ??? jak počítat

    void recursion(unsigned long depth, bool bishopTurn, Chessboard chessboard, std::vector<int> turnMoves) {
        if (shouldExit) {
            std::cout << depth << " - " << turnMoves << std::endl;
            return;
        }

        if (depth > 5) shouldExit = true;

        std::vector<Move> moves = getAllMoves(bishopTurn);
        std::sort(moves.begin(), moves.end());

        for (Move move: moves) {
            Chessboard chessboardForMove = chessboard.clone();
            captureSquare(chessboardForMove, bishopTurn, move.coords);

            std::vector<int> turnMovesForMove = turnMoves;
            turnMovesForMove.push_back(coordsToIndex(chessboard.size, move.coords));

            recursion(depth + 1, !bishopTurn, chessboardForMove);
        }

        std::cout << depth << std::endl;
    }

    void captureSquare(Chessboard chessboard, bool bishopTurn, Coords coords) const {
        chessboard.squares[coords.row * chessboard.size + coords.column] = false;
        chessboard.enemiesCount--;

        if (bishopTurn) {
            chessboard.bishopPosition = coordsToIndex(chessboard.size, coords);
        } else {
            chessboard.knightPosition = coordsToIndex(chessboard.size, coords);
        }
    }

    int coordsToIndex(int size, Coords coords) const {
        return coords.row * size + coords.column;
    }

    bool canGetSquare(Chessboard chessboard, Coords coords) const {
        return !(coords.row < 0 || coords.row >= chessboard.size || coords.column < 0 ||
                 coords.column >= chessboard.size);
    }

    bool getSquare(Chessboard chessboard, Coords coords) const {
        return chessboard.squares[coordsToIndex(chessboard.size, coords)];
    }

    std::vector<Move> getAllMoves(bool bishopTurn) const {
        if (bishopTurn) {
            return getAllBishopMoves();
        }

        return getAllKnightMoves();
    }

    std::vector<Move> getAllBishopMoves() const {
        // TODO:
    }

    std::vector<Move> getAllKnightMoves() const {
        // TODO:
    }
};

#endif //INC_01_SOLVER_HPP
