#include "BoardRaw.h"
#include "Util.h"
#include <memory>

int getNumBitsPerTile(int boardSize);

struct BoardRep {
    BoardRep(const BoardRaw &board);
    std::unique_ptr<uint8_t[]> grid;
    uint8_t boardSize;

    BoardRep(const BoardRep &other);

    BoardRaw toBoard() const;

    BoardRep& operator=(const BoardRep& other) {
        boardSize = other.boardSize;
        grid.reset(new uint8_t[getArraySize()]);
        for (auto i = 0; i < getArraySize(); ++i) grid[i] = other.grid[i];
        return *this;
    }

    int getArraySize() const { return (getNumBitsPerTile(boardSize) * boardSize + 7) / 8; }

    friend bool operator==(const BoardRep &lhs, const BoardRep &rhs) {
        for (auto i = 0; i < lhs.getArraySize(); ++i) {
            if (lhs.grid[i] != rhs.grid[i]) return false;
        }
        return true;
    }
    
    friend bool operator!=(const BoardRep &lhs, const BoardRep &rhs) {
        return !(lhs == rhs);
    }
};


template<>
struct std::hash<BoardRep> {
    std::size_t operator()(BoardRep const& boardRep) const noexcept {
        // see https://stackoverflow.com/questions/20511347/a-good-hash-function-for-a-vector
        std::size_t seed = boardRep.getArraySize();
        for(auto i = 0; i < boardRep.getArraySize(); ++i) {
            seed ^= boardRep.grid[i] + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};
