#ifndef INC_01_PARSER_HPP
#define INC_01_PARSER_HPP

#include <string>
#include <fstream>
#include <vector>
#include "Chessboard.hpp"

#define enemyFigure 'P'
#define bishopFigure 'S'
#define knightFigure 'J'

class Parser {
public:
    Chessboard parse(const std::string &fileName) const {
        std::ifstream file(fileName);
        if (!file.is_open()) {
            std::cout << "Error: Couldn't open the input file." << std::endl;
            exit(2);
        }

        int chessboardSize;
        int maxDepth; // TODO: ?

        file >> chessboardSize >> maxDepth;

        Coords bishopCoords;
        Coords knightCoords;
        std::vector<bool> squares(chessboardSize * chessboardSize, false);
        int enemiesCount = 0;

        char figure;
        for (int row = 0; row < chessboardSize; row++) {
            for (int column = 0; column < chessboardSize; column++) {
                file >> figure;


                squares.push_back(figure == enemyFigure);

                switch (figure) {
                    case enemyFigure:
                        enemiesCount++;
                        break;
                    case bishopFigure:
                        bishopCoords = Coords(row, column);
                        break;
                    case knightFigure:
                        knightCoords = Coords(row, column);
                        break;
                    default:
                        break;
                }
            }
        }

        return Chessboard(chessboardSize, enemiesCount, squares,
                          coordsToIndex(chessboardSize, bishopCoords),
                          coordsToIndex(chessboardSize, knightCoords)
        );
    }

    int coordsToIndex(int size, Coords coords) const {
        return coords.row * size + coords.column;
    }
};

#endif //INC_01_PARSER_HPP
