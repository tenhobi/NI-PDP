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

class Solver {
public:
    explicit Solver(int maxDepth) : maxDepth(maxDepth) {}

    void solve(Chessboard &chessboard) {
        int procNum, procCount;
        MPI_Comm_rank(MPI_COMM_WORLD, &procNum);
        MPI_Comm_size(MPI_COMM_WORLD, &procCount);

        taskBufferSize = 4 + 200 + maxDepth + 2 + 5;
        resultBufferSize = 1 + maxDepth + 1 + 5;

        if (procNum == SOURCE_MASTER) {
            std::cout << "master id=" << procNum << std::endl;
            master(chessboard, procCount);
        } else {
            std::cout << "slave id=" << procNum << std::endl;
            slave(chessboard.size, procCount);
        }
    }

private:
    int maxDepth;
    unsigned long calls = 0;

    int bestPrice = std::numeric_limits<int>::max();
    std::vector<int> bestTurnMoves;

    int taskBufferSize;
    int resultBufferSize;

    void master(Chessboard &chessboard, int procCount) {
        int workerCount = procCount - 1;

        // Generate initial master states.
        std::vector<State> states;
        states = generateBoard(chessboard, procCount * 5, 5);
        std::queue<State> queue;

        for (auto &elem : states) {
            queue.push(std::move(elem));
        }
        states.clear();

        // Send initial task
        for (int i = 1; i <= workerCount && !queue.empty(); i++) {
            State task = queue.front();
            queue.pop();
            std::vector<int> serialized = task.serializeTask(bestPrice);
            MPI_Send(serialized.data(), (int) serialized.size(), MPI_INT, i, TAG_DO_TASK, MPI_COMM_WORLD);
        }

        while (workerCount > 0) {
            std::vector<int> buffer(resultBufferSize);
            MPI_Status status;
            MPI_Recv(buffer.data(), resultBufferSize, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            if (status.MPI_TAG == TAG_TASK_DONE) {
                if (buffer[0] < bestPrice) {
                    bestPrice = buffer[0];

                    std::vector<int> newBestTurnsMoves;
                    for (int i = 1; i < resultBufferSize; i++) {
                        int val = buffer[i];
                        if (val == -1) {
                            break;
                        }
                        newBestTurnsMoves.push_back(val);
                    }
                    bestTurnMoves = newBestTurnsMoves;
                }

                // If there is no more job, kill this slave process.
                if (queue.empty()) {
                    int nothing = 0;
                    MPI_Send(&nothing, 1, MPI_INT, status.MPI_SOURCE, TAG_KILL, MPI_COMM_WORLD);
                    workerCount--;
                    continue;
                }

                // Otherwise send another task.
                State task = queue.front();
                queue.pop();
                std::vector<int> serialized = task.serializeTask(bestPrice);
                MPI_Send(serialized.data(), (int) serialized.size(), MPI_INT, status.MPI_SOURCE, TAG_DO_TASK,
                         MPI_COMM_WORLD);
            }
        }

        std::cout << std::endl;
        std::cout << "Price: " << bestPrice << std::endl;
        std::cout << "Number of calls: " << calls << std::endl;
        std::cout << "Turns:" << std::endl;

        int turnIndex = 1;
        for (int move: bestTurnMoves) {
            Coords moveCoords = chessboard.indexToCoords(move);
            std::cout << (turnIndex % 2 == 1 ? 'S' : 'J') << " -> " << moveCoords
                      << (chessboard.getSquare(moveCoords) ? '*' : ' ') << std::endl;
            turnIndex++;
            chessboard.setSquare(chessboard.indexToCoords(move), false);
        }
    }

    void slave(int chessboardSize, int procCount) {
        while (true) {
            // Read task data
            std::vector<int> buffer(taskBufferSize);
            MPI_Status status;
            MPI_Recv(buffer.data(), taskBufferSize, MPI_INT, SOURCE_MASTER, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            // End processing.
            if (status.MPI_TAG == TAG_KILL) {
                break;
            }

            // Process received task.
            if (status.MPI_TAG == TAG_DO_TASK) {
                State newState = State::parseSerializedTask(buffer, chessboardSize);

                std::vector<State> states;
                states = generateBoard(newState.chessboard, std::thread::hardware_concurrency() * 1000,newState.depth + 5, newState.depth);

                #pragma omp parallel for schedule(dynamic) default(none) shared(states)
                for (auto &state : states) {
                    recursionSeq(state);
                }

                std::vector<int> serializedResult = serializeResult(bestPrice, bestTurnMoves);
                MPI_Send(serializedResult.data(), (int) serializedResult.size(), MPI_INT, SOURCE_MASTER, TAG_TASK_DONE,
                         MPI_COMM_WORLD);
            }
        }
    }

    // --- RESULT
    // bestPrice
    // turn moves [] array ... max maxDepth
    // ---- -1 if array ended
    // +1 for -1 arrays' endings
    std::vector<int> serializeResult(int price, std::vector<int> turnMoves) {
        std::vector<int> buffer;

        buffer.push_back(price);

        for (const auto &item : turnMoves) {
            buffer.push_back(item);
        }
        buffer.push_back(-1);

        return buffer;
    }

    std::vector<State> generateBoard(Chessboard &chessboard, unsigned int count, int depthLimit, int currentDepth = 0) {
        std::vector<State> states; // output vector

        std::queue<State> statesQueue;
        State x = State(currentDepth, chessboard.clone(), std::vector<int>());
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
