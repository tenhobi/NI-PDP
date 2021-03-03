#ifndef INC_01_COORDS_HPP
#define INC_01_COORDS_HPP

class Coords {
public:
    int row;
    int column;

    Coords() : row(-1), column(-1) {}

    Coords(int row, int column) : row(row), column(column) {}
};

#endif //INC_01_COORDS_HPP
