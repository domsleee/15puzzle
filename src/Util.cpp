#include "../include/Util.h"

#include <algorithm>

#include "../include/Direction.h"
#include "../include/BoardRaw.h"

std::vector<int> combine(const std::vector<std::vector<int>>& grids) {
    std::vector<int> solution(grids[0].size(), 0);
    for (auto& grid : grids) {
        assertm(grid.size() == solution.size(), "Mismatching pattern sizes");
        for (std::size_t i = 0; i < grid.size(); i++) {
            if (grid[i] != 0) {
                solution[i] = grid[i];
            }
        }
    }
    return solution;
}

int getBlank(const std::vector<int>& board) {
    auto it = std::find(board.cbegin(), board.cend(), 0);
    assertm(it != board.cend(), "Blank must exist in board");
    return std::distance(board.cbegin(), it);
}

std::vector<std::array<bool, 4>> calcMoveList(int width, int height) {
    // [index][direction]
    std::vector<std::array<bool, 4>> moves(width * height,
                                           std::array<bool, 4>{});

    // Blank position
    for (int i = 0; i < width * height; i++) {
        moves[i][static_cast<int>(Direction::U)] = (i / width) > 0;
        moves[i][static_cast<int>(Direction::R)] = (i % width) < width - 1;
        moves[i][static_cast<int>(Direction::D)] = (i / width) < height - 1;
        moves[i][static_cast<int>(Direction::L)] = (i % width) > 0;
    }

    return moves;
}

Direction inverse(Direction move) {
    switch (move) {
        case Direction::U:
            return Direction::D;
        case Direction::L:
            return Direction::R;
        case Direction::D:
            return Direction::U;
        case Direction::R:
            return Direction::L;
        default:
            assertm(0, "Unknown direction in inverse");
    }
}

char directionToChar(Direction move) {
    switch (move) {
        case Direction::U: return 'u';
        case Direction::D: return 'd';
        case Direction::R: return 'r';
        case Direction::L: return 'l';
    }
    assertm(0, "unknown direction");
}

int charToInt(char move) {
    return static_cast<int>(charToDirection(move));
}

Direction charToDirection(char move) {
    switch (move) {
        case 'u': return Direction::U;
        case 'd': return Direction::D;
        case 'r': return Direction::R;
        case 'l': return Direction::L;
    }
    assertm(0, "Unknown move in charToDirection");
}

std::vector<BoardRaw> getAllStartingBoards(int width, int height) {
    std::vector<BoardRaw> res;
    std::vector<int> boardVec(width*height);
    for (int i = 0; i < (width * height) - 1; ++i) boardVec[i] = i+1;
    boardVec[(width * height) - 1] = 0;

    auto startBoard = BoardRaw(boardVec, width, height);
    res.push_back(startBoard);
    auto lastIndex = startBoard.grid.size()-1;

    for (auto i = 0; i < lastIndex; ++i) {
        auto newGrid = startBoard.grid;
        std::swap(newGrid[i], newGrid[lastIndex]);
        res.push_back(BoardRaw(newGrid, width, height));
    }

    return res;
}