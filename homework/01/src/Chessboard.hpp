#ifndef INC_01_CHESSBOARD_HPP
#define INC_01_CHESSBOARD_HPP

#include <utility>
#include <vector>
#include "Coords.hpp"

class Chessboard {
public:
    int enemiesCount;
    int size;
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

    Chessboard clone() {
        std::vector<bool> squaresCopy = squares;
        return Chessboard(size, enemiesCount, squaresCopy, bishopPosition, knightPosition);
    }
};

#endif //INC_01_CHESSBOARD_HPP
