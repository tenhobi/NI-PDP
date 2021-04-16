#ifndef INC_03_STATE_HPP
#define INC_03_STATE_HPP

#include <utility>

#include "Chessboard.hpp"

class State {
public:
    int depth;
    Chessboard chessboard;
    std::vector<int> turnMoves;

    State(const int depth,
          const Chessboard &chessboard,
          std::vector<int> turnMoves) : depth(depth),
                                        chessboard(std::move(chessboard)),
                                        turnMoves(std::move(turnMoves)) {}

    bool operator<(const State &other) const {
        if (chessboard.enemiesCount == other.chessboard.enemiesCount) {
            return depth < other.depth;
        }
        return chessboard.enemiesCount < other.chessboard.enemiesCount;
    }

    bool operator==(const State &rhs) const {
        return depth == rhs.depth &&
               chessboard == rhs.chessboard;
    }

    bool operator!=(const State &rhs) const {
        return !(rhs == *this);
    }
};

#endif //INC_03_STATE_HPP
