#include "../include/Util.h"

#include <algorithm>
#include <fstream>
#include <filesystem>

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
    // if (it == board.cend()) {
    //     DEBUG("BOARD SIZE: " << board.size());
    //     for (auto c: board) DEBUG(c);
    // }
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
            exit(1);
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
    DEBUG("char given " << move);
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

    for (std::size_t i = 0; i < lastIndex; ++i) {
        auto newGrid = startBoard.grid;
        std::swap(newGrid[i], newGrid[lastIndex]);
        res.push_back(BoardRaw(newGrid, width, height));
    }

    return res;
}

void writeWordsToFile(std::string filename, const std::unordered_set<std::string> &words) {
    if (std::filesystem::exists(filename)) {
        DEBUG("refused to write words to file that already existed " << filename);
        return;
    }
    std::ofstream fout{filename};
    std::vector<std::string> vec(words.begin(), words.end());
    std::sort(vec.begin(), vec.end(), StringVectorCompare());
    for (auto s: vec) fout << s << '\n';
}

bool readWordsFromFile(std::string filename, std::unordered_set<std::string> &words) {
    if (!std::filesystem::exists(filename)) {
        return false;
    }
    std::ifstream fin{filename};
    std::string s;
    while (fin >> s) words.insert(s);
    return true;
}

Direction pathMoved(const BoardRaw& a, const BoardRaw& b)
{
    auto diff = a.getBlankTile() - b.getBlankTile();
    switch(diff) {
        case 1: return Direction::L;
        case -1: return Direction::R;
        default: {
            if (diff == -b.getWidth()) return Direction::U;
            else if (diff == b.getWidth()) return Direction::D;
            DEBUG("BLANKS " << a.getBlankTile() << " VS " << b.getBlankTile() << ", diff " << diff << " width" << a.getWidth());
            assertm(0, "no direction found??");
        }
    }
}

int numBitsPerTileCache[1024] = {0};

int getNumBitsPerTile(int boardSize) {
    if (numBitsPerTileCache[boardSize] != 0) {
        return numBitsPerTileCache[boardSize];
    }
    return numBitsPerTileCache[boardSize] = getNumBitsPerTileConst(boardSize);
}

int getBitmask(int bitsPerTile) {
    switch(bitsPerTile) {
        case 0: return 0;
        case 1: return 0b1;
        case 2: return 0b11;
        case 3: return 0b111;
        case 4: return 0b1111;
        case 5: return 0b11111;
        case 6: return 0b111111;
        case 7: return 0b1111111;
        case 8: return 0b11111111;
    }
    throw "not implemented";
}
