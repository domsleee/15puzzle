#pragma once
#include "BoardRaw.h"
#include "Util.h"
#include <memory>

template <int WIDTH>
struct BoardRepTempl {
    BoardRepTempl(const BoardRaw &board);
    BoardRepTempl() { throw "what"; }
    std::array<uint8_t, (getNumBitsPerTileConst(WIDTH*WIDTH) * (WIDTH*WIDTH) + 7) / 8> grid;

    BoardRaw toBoard() const;

    template<int WIDTH2>
    friend bool operator==(const BoardRepTempl<WIDTH> &lhs, const BoardRepTempl<WIDTH2> &rhs) {
        if (WIDTH != WIDTH2) return false;
        for (std::size_t i = 0; i < lhs.grid.size(); ++i) {
            if (lhs.grid[i] != rhs.grid[i]) return false;
        }
        return true;
    }

    template<int WIDTH2>
    friend bool operator!=(const BoardRepTempl<WIDTH> &lhs, const BoardRepTempl<WIDTH2> &rhs) {
        return !(lhs == rhs);
    }

    template<int WIDTH2>
    friend bool operator<(const BoardRepTempl<WIDTH> &a, const BoardRepTempl<WIDTH2>& b) {
        //DEBUG("WIDTH: " << WIDTH<< ", WIDTH2: " << WIDTH2);
        if (WIDTH != WIDTH2) return WIDTH < WIDTH2;
        for (std::size_t i = 0; i < a.grid.size(); ++i) {
            if (a.grid[i] != b.grid[i]) return a.grid[i] < b.grid[i];
        }
        return false;
    }
};
