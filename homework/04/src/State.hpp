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

    // --- DATA
    // currentBestPrice
    // depth
    // bishop position
    // knight position
    // pawns positions [] array ... max 200
    // turn moves [] array ... max maxDepth
    // ---- -1 if array ended
    // +2 for -1 arrays' endings
    std::vector<int> serializeTask(int bestPrice) {
        std::vector<int> buffer;

        buffer.push_back(bestPrice);
        buffer.push_back(depth);
        buffer.push_back(chessboard.bishopPosition);
        buffer.push_back(chessboard.knightPosition);

        for (int row = 0; row < chessboard.size; row++) {
            for (int column = 0; column < chessboard.size; column++) {
                Coords coords(row, column);
                if (chessboard.getSquare(coords)) {
                    buffer.push_back(Chessboard::coordsToIndex(chessboard.size, coords));
                }
            }
        }
        buffer.push_back(-1);

        for (const auto &item : turnMoves) {
            buffer.push_back(item);
        }
        buffer.push_back(-1);

        return buffer;
    }

    static State parseSerializedTask(std::vector<int> serializedData, int chessboardSize) {
        int newDepth = serializedData[1];
        int bishopPosition = serializedData[2];
        int knightPosition = serializedData[3];

        int i = 4;
        std::vector<int> pawns;
        while (serializedData[i] != -1) {
            pawns.push_back(serializedData[i]);
            i++;
        }

        i++;

        std::vector<bool> squares(chessboardSize * chessboardSize, false);
        for (const auto &item : pawns) {
            squares[item] = true;
        }

        Chessboard newChessboard(chessboardSize, (int) pawns.size(), squares, bishopPosition, knightPosition);

        std::vector<int> newTurnMoves;
        while (serializedData[i] != -1) {
            newTurnMoves.push_back(serializedData[i]);
            i++;
        }

        return State(newDepth, newChessboard, newTurnMoves);
    }
};

#endif //INC_03_STATE_HPP
