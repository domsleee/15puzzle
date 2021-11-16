#include "../include/BoardRaw.h"

#include <algorithm>
#include <execution>
#include <functional>
#include <iomanip>
#include <numeric>
#include <unordered_map>

#include "../include/Util.h"
#include "../include/WalkingDistance.h"

// WIDTH < HEIGHT

BoardRaw::BoardRaw(const std::vector<int>& g, int width, int height)
    : WIDTH(width),
      HEIGHT(height),
      deltas({-width, 1, width, -1}),
      canMoveList(calcMoveList(width, height)),
      blank(getBlank(g)),
      grid(g)
{}

std::vector<Direction> BoardRaw::getMoves() const {
    if (blank < WIDTH) {           // top
        if (blank % WIDTH == 0) {  // left
            return {Direction::R, Direction::D};
        }
        if (blank % WIDTH == WIDTH - 1) {  // right
            return {Direction::D, Direction::L};
        }
        return {Direction::R, Direction::D, Direction::L};
    }
    if (blank >= (WIDTH - 1) * HEIGHT) {  // bottom
        if (blank % WIDTH == 0) {         // left
            return {Direction::U, Direction::R};
        }
        if (blank % WIDTH == WIDTH - 1) {  // right
            return {Direction::U, Direction::L};
        }
        return {Direction::U, Direction::R, Direction::L};
    }
    if (blank % WIDTH == 0) {  // left
        return {Direction::U, Direction::R, Direction::D};
    }
    if (blank % WIDTH == WIDTH - 1) {  // right
        return {Direction::U, Direction::D, Direction::L};
    }

    return {Direction::U, Direction::R, Direction::D, Direction::L};
}

inline int BoardRaw::getTile(int posn) const { return grid[posn]; }

inline void BoardRaw::setTile(int posn, int tile) { grid[posn] = tile; }

bool BoardRaw::canMove(Direction dir) {
    return canMoveList[blank][static_cast<int>(dir)];
}

BoardRaw::MoveState BoardRaw::applyMove(Direction dir) {
    // Position of sliding tile (and new blank)
    const auto newBlank = blank + deltas[static_cast<int>(dir)];
    // Value of sliding tile
    const auto tile = getTile(newBlank);

    // Set value of slid tile
    setTile(blank, tile);

    // Update blank tile
    const auto oldBlank = blank;
    blank = newBlank;

    return {oldBlank};
}

void BoardRaw::undoMove(const BoardRaw::MoveState& prev) {
    const auto& [newBlank] = prev;

    // Value of sliding tile
    const auto tile = getTile(newBlank);

    // Set value of slid tile
    setTile(blank, tile);

    // Update blank tile
    blank = newBlank;
}

std::ostream& operator<<(std::ostream& out, const BoardRaw& board) {
    for (int y = 0; y < board.HEIGHT; y++) {
        for (int x = 0; x < board.WIDTH; x++) {
            int i = y * board.WIDTH + x;
            if (i == board.blank) {
                out << std::setw(3) << 0;
            } else {
                out << std::setw(3) << board.getTile(i);
            }
        }
        out << std::endl;
    }
    return out;
}
