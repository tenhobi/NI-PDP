#ifndef INC_01_SOLVER_HPP
#define INC_01_SOLVER_HPP

#include <algorithm>
#include <thread>
#include <queue>
#include <set>

#include "Chessboard.hpp"
#include "Move.hpp"
#include "State.hpp"

#define SOURCE_MASTER 0

#define TAG_DO_TASK 1
#define TAG_TASK_DONE 2
#define TAG_KILL 3
//#define TAG_M_SEND_RESULTS 4
//#define TAG_S_RESULTS_SENT 5

class Solver {
public:
    explicit Solver(int maxDepth) : maxDepth(maxDepth) {}

    void solve(Chessboard &chessboard) {
        int procNum, procCount;
        MPI_Comm_rank(MPI_COMM_WORLD, &procNum);
        MPI_Comm_size(MPI_COMM_WORLD, &procCount);

        if (procNum == 0) {
            master(chessboard, procCount);
        } else {
            slave();
        }
    }

private:
    void master(Chessboard &chessboard, int procCount) {
        int workerCount = procCount - 1;

        // Generate initial master states.
        std::vector<State> states;
        states = generateBoard(chessboard, std::thread::hardware_concurrency() * 1000, 4);
        std::cout << "States generated: " << states.size() << std::endl;
        std::queue<State> queue;

        for (auto &elem : states) {
            queue.push(std::move(elem));
        }
        states.clear();

        // Send initial task
        for (int i = 1; i <= workerCount && !queue.empty(); i++) {
            // TODO
        }

        while (workerCount > 0) {
            // --- RESULT
            // bestPrice
            // turn moves [] array ... max maxDepth
            // ---- -1 if array ended
            // +1 for -1 arrays' endings
            // +5 additional just for sure
            int bufferSize = 4 + 200 + maxDepth + 2 + 5;
            std::vector<int> buffer(bufferSize);
            MPI_Status status;
            MPI_Recv(buffer.data(), bufferSize, MPI_INT, SOURCE_MASTER, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            if (status.MPI_TAG == TAG_TASK_DONE) {
                // TODO: process data, update etc.

                if (queue.empty()) {
                    int nothing = 0;
                    MPI_Send(&nothing, 1, MPI_INT, status.MPI_SOURCE, TAG_KILL, MPI_COMM_WORLD);
                    workerCount--;
                    continue;
                }

                // --- DATA
                // currentBestPrice
                // chessboardSize
                // depth
                // bishop position
                // knight position
                // pawns positions [] array ... max 200
                // turn moves [] array ... max maxDepth
                // ---- -1 if array ended
                // +2 for -1 arrays' endings
                // +5 additional just for sure
                int taskBufferSize = 5 + 200 + maxDepth + 2 + 5;
                std::vector<int> taskBuffer(taskBufferSize);
                MPI_Status taskStatus;
                // TODO: send another task
                int nothing = 0;
                MPI_Send(&nothing, 1, MPI_INT, status.MPI_SOURCE, TAG_DO_TASK, MPI_COMM_WORLD);
            } else {
                std::cout << "WTF??" << std::endl;
            }
        }

        // TODO

    }

    void slave() {
        // --- DATA
        // currentBestPrice
        // chessboardSize
        // depth
        // bishop position
        // knight position
        // pawns positions [] array ... max 200
        // turn moves [] array ... max maxDepth
        // ---- -1 if array ended
        // +2 for -1 arrays' endings
        // +5 additional just for sure
        int bufferSize = 5 + 200 + maxDepth + 2 + 5;

        while (true) {
            // Read data
            std::vector<int> buffer(bufferSize);
            MPI_Status status;
            MPI_Recv(buffer.data(), bufferSize, MPI_INT, SOURCE_MASTER, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            // End processing.
            if (status.MPI_TAG == TAG_KILL) {
                break;
            }

            // Process received task.
            if (status.MPI_TAG == TAG_DO_TASK) {
                // TODO: parse received data

                std::vector<State> states;
                states = generateBoard(chessboard, std::thread::hardware_concurrency() * 1000, taskDepth + 4);

                #pragma omp parallel for schedule(dynamic) default(none) shared(states)
                for (auto &state : states) {
                    recursionSeq(state);
                }

                // std::cout << "Price: " << bestPrice << std::endl;
                // std::cout << "Number of calls: " << calls << std::endl;
                // std::cout << "Turns:" << std::endl;

                int turnIndex = 1;
                for (int move: bestTurnMoves) {
                    Coords moveCoords = chessboard.indexToCoords(move);
                    std::cout << (turnIndex % 2 == 1 ? 'S' : 'J') << " -> " << moveCoords
                              << (chessboard.getSquare(moveCoords) ? '*' : ' ') << std::endl;
                    turnIndex++;
                }
                
                // --- RESULT
                // bestPrice
                // turn moves [] array ... max maxDepth
                // ---- -1 if array ended
                // +1 for -1 arrays' endings
                // +5 additional just for sure
                int outputBufferSize = 1 + maxDepth + 1 + 5;
                std::vector<int> outputBuffer(outputBufferSize);
                // TODO: serialize data to output buffer
                int nothing = 0;
                MPI_Send(&nothing, 1, MPI_INT, status.MPI_SOURCE, TAG_TASK_DONE, MPI_COMM_WORLD);
            }
        }
    }

    int maxDepth;
    unsigned long calls = 0;

    int bestPrice = std::numeric_limits<int>::max();
    std::vector<int> bestTurnMoves;

    std::vector<State> generateBoard(Chessboard &chessboard, unsigned int count, int depthLimit) {
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
            if (currentState.depth >= depthLimit) {
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
