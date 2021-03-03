#ifndef INC_01_PARSER_HPP
#define INC_01_PARSER_HPP

#include <string>
#include <fstream>
#include <utility>
#include <vector>
#include "Chessboard.hpp"

#define enemyFigure 'P'
#define bishopFigure 'S'
#define knightFigure 'J'

class ParserOutput {
public:
    unsigned long maxDepth;
    Chessboard chessboard;

    ParserOutput(unsigned long maxDepth, Chessboard chessboard) : maxDepth(maxDepth), chessboard(std::move(chessboard)) {}
};

class Parser {
public:
    ParserOutput parse(const std::string &fileName) const {
        std::ifstream file(fileName);
        if (!file.is_open()) {
            std::cout << "Error: Couldn't open the input file." << std::endl;
            exit(2);
        }

        int chessboardSize;
        unsigned long maxDepth;

        file >> chessboardSize >> maxDepth;

        Coords bishopCoords;
        Coords knightCoords;
        std::vector<bool> squares(chessboardSize * chessboardSize, false);
        int enemiesCount = 0;

        char figure;
        for (int row = 0; row < chessboardSize; row++) {
            for (int column = 0; column < chessboardSize; column++) {
                file >> figure;

                if (figure == enemyFigure) {
                    squares[Chessboard::coordsToIndex(chessboardSize, Coords(row, column))] = true;
                }

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

        return ParserOutput(maxDepth,
                            Chessboard(chessboardSize, enemiesCount, squares,
                                       Chessboard::coordsToIndex(chessboardSize, bishopCoords),
                                       Chessboard::coordsToIndex(chessboardSize, knightCoords))
        );
    }
};

#endif //INC_01_PARSER_HPP
