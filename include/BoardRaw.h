#ifndef BOARDRAW_H
#define BOARDRAW_H

#include <array>
#include <ostream>
#include <vector>

#include "Direction.h"
#include "DisjointDatabase.h"

/*
    Rectangular board
    - No mirror
    - No walking distance
    - No patterns
    - No heuristic
*/

class BoardRaw {
    const int WIDTH;
    const int HEIGHT;

    // {0, -1}, {1, 0}, {0, 1}, {-1, 0}}
    const std::array<int, 4> deltas;  // Blank deltas

    // Faster than computing on the fly
    const std::vector<std::array<bool, 4>> canMoveList;

    // Tiles
    int blank;  // Position of blank (since patterns don't store the blank)

    int getTile(int posn) const;
    void setTile(int posn, int tile);

    int getDelta(const std::vector<int>& g, int tile, int offset) const;

public:
    typedef int MoveState;

    BoardRaw(const std::vector<int>& g, int width, int height);
    std::vector<int> grid;  // Value to position mapping

    bool canMove(Direction dir);
    // Should be run only once at start of search
    std::vector<Direction> getMoves() const;
    BoardRaw::MoveState applyMove(Direction dir);
    void undoMove(BoardRaw::MoveState prev);

    friend std::ostream& operator<<(std::ostream& out, const BoardRaw& board);
    friend bool operator==(const BoardRaw &lhs, const BoardRaw &rhs) {
        return lhs.grid == rhs.grid;
    }

    int getBlankTile() const { return blank; }
    int getWidth() const { return WIDTH; }
    int getMiddleTile() const { return (WIDTH*HEIGHT) / 2; }
};


template<>
struct std::hash<BoardRaw> {
    std::size_t operator()(BoardRaw const& board) const noexcept {
        // see https://stackoverflow.com/questions/20511347/a-good-hash-function-for-a-vector
        std::size_t seed = board.grid.size();
        for(auto& i : board.grid) {
            seed ^= i + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};



#endif  // BOARDRECT_H
