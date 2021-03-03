#ifndef INC_01_MOVE_HPP
#define INC_01_MOVE_HPP

#include "Coords.hpp"

class Move {
public:
    Coords coords;
    int value;

    Move(const Coords &coords, int value) : coords(coords), value(value) {}

    bool operator<(const Move &other) const {
        return value > other.value;
    }
};

#endif //INC_01_MOVE_HPP
