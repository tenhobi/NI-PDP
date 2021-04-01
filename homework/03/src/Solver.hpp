#ifndef INC_01_SOLVER_HPP
#define INC_01_SOLVER_HPP

#include <algorithm>
#include <thread>
#include <queue>
#include <set>

#include "Chessboard.hpp"
#include "Move.hpp"
#include "State.hpp"

class Solver {
public:
    explicit Solver(int maxDepth) : maxDepth(maxDepth) {}

    void solve(Chessboard &chessboard) {
        std::vector<State> states;

        states = generateBoard(chessboard, std::thread::hardware_concurrency() * 1000);
        std::cout << "States generated: " << states.size() << std::endl;

        #pragma omp parallel for schedule(dynamic) default(none) shared(states)
        for (auto &state : states) {
            recursionSeq(state);
        }

        std::cout << "Price: " << bestPrice << std::endl;
        std::cout << "Number of calls: " << calls << std::endl;
        std::cout << "Turns:" << std::endl;

        int turnIndex = 1;
        for (int move: bestTurnMoves) {
            Coords moveCoords = chessboard.indexToCoords(move);
            std::cout << (turnIndex % 2 == 1 ? 'S' : 'J') << " -> " << moveCoords
                      << (chessboard.getSquare(moveCoords) ? '*' : ' ') << std::endl;
            turnIndex++;
        }
    }

private:
    int maxDepth;
    unsigned long calls = 0;

    int bestPrice = std::numeric_limits<int>::max();
    std::vector<int> bestTurnMoves;

    std::vector<State> generateBoard(Chessboard &chessboard, unsigned int count) {
        std::vector<State> states; // output vector

        std::queue<State> statesQueue;
        State x = State(0, chessboard.clone(), std::vector<int>());
        statesQueue.push(x);

        while (statesQueue.size() < count) {
            // Get first value.
            State currentState = statesQueue.front();
            // Solution found, break.
            if (currentState.chessboard.enemiesCount == 0) {
                break;
            }
            // Don't generate more deep states.
            if (currentState.depth >= 7) {
                break;
            }
            statesQueue.pop();

            // Generate all next moves from this state.
            std::vector<Move> moves = currentState.chessboard.getAllMoves(currentState.depth % 2 == 0);

            // Push all new moves to queue.
            for (auto &move: moves) {
                Chessboard newChessboard = currentState.chessboard.clone();
                std::vector<int> newTurnMoves = currentState.turnMoves;

                newChessboard.moveToSquare(currentState.depth % 2 == 0, move.coords);
                newTurnMoves.emplace_back(Chessboard::coordsToIndex(newChessboard.size, move.coords));

                statesQueue.push(State(currentState.depth + 1, newChessboard, newTurnMoves));
            }
        }

        while (!statesQueue.empty()) {
            states.emplace_back(std::move(statesQueue.front()));
            statesQueue.pop();
        }

        // Sort
        std::sort(states.begin(), states.end());

        // Remove duplicates
        auto last = std::unique(states.begin(), states.end());
        states.erase(last, states.end());

        return states;
    }

    void recursionSeq(State &state) {
        calls++;

        if (shouldExit(state.depth, state.chessboard, state.turnMoves)) {
            return;
        }

        std::vector<Move> moves = state.chessboard.getAllMoves(state.depth % 2 == 0);
        std::sort(moves.begin(), moves.end());

        for (auto &move: moves) {
            Coords currentPosition = state.chessboard.currentPlayerPosition(state.depth % 2 == 0);
            bool currentMoveValue = state.chessboard.getSquare(move.coords);

            // do changes
            state.chessboard.moveToSquare(state.depth % 2 == 0, move.coords);
            state.turnMoves.emplace_back(Chessboard::coordsToIndex(state.chessboard.size, move.coords));
            state.depth++;

            recursionSeq(state);

            // undo changes
            state.depth--;
            state.turnMoves.pop_back();
            state.chessboard.moveToSquare(state.depth % 2 == 0, currentPosition);
            state.chessboard.setSquare(move.coords, currentMoveValue);
            if (currentMoveValue) {
                state.chessboard.enemiesCount++;
            }
        }
    }

    bool shouldExit(int depth, const Chessboard &chessboard, const std::vector<int> &turnMoves) {
        if (chessboard.enemiesCount <= 0) {
            if (bestPrice > depth) {
                # pragma omp critical
                if (bestPrice > depth) {
                    // std::cout << "hit " << depth << std::endl;
                    bestPrice = depth;
                    bestTurnMoves = turnMoves;
                }
            }
            return true;
        }

        if (depth >= maxDepth) {
            return true;
        }

        if (depth + chessboard.enemiesCount >= maxDepth) {
            return true;
        }

        if (depth + chessboard.enemiesCount >= bestPrice) {
            return true;
        }

        return false;
    }

};

#endif //INC_01_SOLVER_HPP
