#ifndef INC_01_COORDS_HPP
#define INC_01_COORDS_HPP

#include <ostream>

class Coords {
public:
    int row;
    int column;

    Coords() : row(-1), column(-1) {}

    Coords(int row, int column) : row(row), column(column) {}

    Coords operator*(const int &value) const {
        return Coords(row * value, column * value);
    }

    Coords operator+(const Coords &other) const {
        return Coords(row + other.row, column + other.column);
    }

    friend std::ostream &operator<<(std::ostream &os, const Coords &coords) {
        os << "[" << coords.row << ", " << coords.column << "]";
        return os;
    }
};

#endif //INC_01_COORDS_HPP
