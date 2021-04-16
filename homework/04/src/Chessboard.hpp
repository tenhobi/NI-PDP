#ifndef INC_01_CHESSBOARD_HPP
#define INC_01_CHESSBOARD_HPP

#include <utility>
#include <vector>
#include <ostream>
#include "Coords.hpp"
#include "Move.hpp"

const std::vector<Coords> bishopMoves = {
        {-1, -1}, // top left
        {-1, 1}, // top right

        {1,  -1}, // bottom left
        {1,  1}, // bottom right
};

const std::vector<Coords> knightMoves = {
        {-2, -1}, // top top left
        {-2, 1}, // top top right

        {-1, 2}, // right right top
        {1,  2}, // right right bottom

        {2,  -1}, // bottom bottom left
        {2,  1}, // bottom bottom right

        {-1, -2}, // right right top
        {1,  -2}, // right right bottom
};

class Chessboard {
public:
    int size;
    int enemiesCount;
    std::vector<bool> squares;
    int bishopPosition;
    int knightPosition;

    Chessboard(int size,
               int enemiesCount,
               std::vector<bool> squares,
               int bishopPosition,
               int knightPosition) :
            size(size),
            enemiesCount(enemiesCount),
            squares(std::move(squares)),
            bishopPosition(bishopPosition),
            knightPosition(knightPosition) {}

    Chessboard(const Chessboard &board) {
        size = board.size;
        enemiesCount = board.enemiesCount;
        squares = board.squares;
        bishopPosition = board.bishopPosition;
        knightPosition = board.knightPosition;
    }

    Chessboard clone() const {
        std::vector<bool> squaresCopy = squares;
        return Chessboard(size, enemiesCount, squaresCopy, bishopPosition, knightPosition);
    }

    bool operator==(const Chessboard &rhs) const {
        return enemiesCount == rhs.enemiesCount &&
//               bishopPosition == rhs.bishopPosition &&
//               knightPosition == rhs.knightPosition &&
               squares == rhs.squares;
    }

    bool operator!=(const Chessboard &rhs) const {
        return !(rhs == *this);
    }

    int coordsToIndex(const Coords &coords) const {
        return Chessboard::coordsToIndex(size, coords);
    }

    Coords indexToCoords(int index) const {
        int column = index % size;
        int row = index / size;
        return Coords(row, column);
    }

    static int coordsToIndex(int size, const Coords &coords) {
        return coords.row * size + coords.column;
    }

    static Coords indexToCoords(int size, int index) {
        int column = index % size;
        int row = index / size;
        return Coords(row, column);
    }

    Coords currentPlayerPosition(bool bishopTurn) const {
        if (bishopTurn) {
            return indexToCoords(bishopPosition);
        } else {
            return indexToCoords(knightPosition);
        }
    }

    void moveToSquare(bool bishopTurn, Coords coords) {
        if (getSquare(coords)) {
            squares[coordsToIndex(coords)] = false;
            enemiesCount--;
        }

        if (bishopTurn) {
            bishopPosition = coordsToIndex(size, coords);
        } else {
            knightPosition = coordsToIndex(size, coords);
        }
    }

    bool canGetSquare(const Coords &coords) const {
        return !(coords.row < 0 || coords.row >= size || coords.column < 0 ||
                 coords.column >= size);
    }

    bool getSquare(const Coords &coords) const {
        return squares[coordsToIndex(coords)];
    }

    void setSquare(const Coords &coords, bool value) {
        if (canGetSquare(coords)) {
            squares[coordsToIndex(coords)] = value;
        }
    }

    bool isPlayerFigure(const Coords &coords) const {
        return coordsToIndex(coords) == bishopPosition || coordsToIndex(coords) == knightPosition;
    }

    bool isBishopFigure(const Coords &coords) const {
        return coordsToIndex(coords) == bishopPosition;
    }

    bool isKnightFigure(const Coords &coords) const {
        return coordsToIndex(coords) == knightPosition;
    }

    std::vector<Move> getAllMoves(bool bishopTurn) const {
        if (bishopTurn) {
            return getAllBishopMoves();
        }

        return getAllKnightMoves();
    }

    std::vector<Move> getAllBishopMoves() const {
        std::vector<Move> moves;
        moves.reserve(2 * size);
        Coords currentBishopPosition = indexToCoords(bishopPosition);

        for (auto &coords: bishopMoves) {
            for (int i = 1; i <= size; i++) {
                Coords moveCoordsTimes = currentBishopPosition + coords * i;

                // Invalid chessboard square.
                if (!canGetSquare(moveCoordsTimes)) break;

                // Player figure is there -- bishop or knight.
                if (isPlayerFigure(moveCoordsTimes)) break;

                // Hit enemy figure.
                if (getSquare(moveCoordsTimes)) {
                    moves.emplace_back(moveCoordsTimes, 2);
                    break;
                }
                    // Enemy present diagonally from this position.
                else if (enemyIsOnDiagonalFromCoords(moveCoordsTimes)) {
                    moves.emplace_back(moveCoordsTimes, 1);
                }
                    // Other.
                else {
                    moves.emplace_back(moveCoordsTimes, 0);
                }
            }
        }

        return moves;
    }

    std::vector<Move> getAllKnightMoves() const {
        std::vector<Move> moves;
        moves.reserve(8);
        Coords currentKnightPosition = indexToCoords(knightPosition);

        for (auto &coords: knightMoves) {
            Coords moveCoords = currentKnightPosition + coords;

            // Invalid chessboard square.
            if (!canGetSquare(moveCoords)) continue;

            // Player figure is there -- bishop or knight.
            if (isPlayerFigure(moveCoords)) continue;

            // Hit enemy figure.
            if (getSquare(moveCoords)) {
                moves.emplace_back(moveCoords, 2);
            }
//            // Enemy present diagonally from this position.
//            else if (enemyIsOnDiagonalFromCoords(moveCoords)) {
//                moves.emplace_back(moveCoords, 1);
//            }
                // Other.
            else {
                moves.emplace_back(moveCoords, 0);
            }
        }

        return moves;
    }

    bool enemyIsOnDiagonalFromCoords(const Coords &coords) const {
        for (auto &moveCoords: bishopMoves) {
            for (int i = 1; i <= size; i++) {
                Coords moveCoordsTimes = coords + moveCoords * i;

                // Invalid chessboard square.
                if (!canGetSquare(moveCoordsTimes)) break;

                // Player figure is there -- bishop or knight.
                if (isPlayerFigure(moveCoordsTimes)) break;

                // Hit enemy figure.
                if (getSquare(moveCoordsTimes)) {
                    return true;
                }
            }
        }

        return false;
    }

    // ---

    friend std::ostream &operator<<(std::ostream &os, const Chessboard &chessboard) {
        for (int row = 0; row < chessboard.size; ++row) {
            for (int column = 0; column < chessboard.size; ++column) {
                Coords currentCoords = Coords(row, column);
                if (chessboard.isPlayerFigure(currentCoords)) {
                    os << (chessboard.isBishopFigure(currentCoords) ? 'S' : 'J') << " ";
                } else {
                    os << (chessboard.getSquare(currentCoords) ? 'P' : '-') << " ";
                }
            }
            os << std::endl;
        }
        return os;
    }
};

#endif //INC_01_CHESSBOARD_HPP
