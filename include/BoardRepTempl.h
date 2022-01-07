#pragma once
#include "BoardRaw.h"
#include "Util.h"
#include <memory>

template <int WIDTH>
struct BoardRepTempl {
    BoardRepTempl(const BoardRaw &board);
    BoardRepTempl(const BoardRaw &board, uint8_t coord);
    BoardRepTempl() { assertm(0, "Not implemented BoardRepTempl"); }

    static constexpr int arraySize = (getNumBitsPerTileConst(WIDTH*WIDTH) * (WIDTH*WIDTH) + 7) / 8;

    std::array<uint8_t, arraySize> grid;

    BoardRaw toBoard() const;

    static int getByteEstimate() {
        return arraySize;
    }

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
