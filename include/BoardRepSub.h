#pragma once
#include "BoardRaw.h"
#include "BoardRepTempl.h"

#include "Util.h"
#include <memory>

template <int WIDTH>
struct BoardRepSub {
    BoardRepSub() { assertm(0, "not implemented BoardRepSub"); }
    BoardRepSub(const BoardRaw &board, uint8_t coord);
    BoardRepSub(const BoardRaw &board) { assertm(0, "not impleneted"); }
    uint8_t coord;
    BoardRepTempl<WIDTH> boardRep;

    template<int WIDTH2>
    friend bool operator==(const BoardRepSub<WIDTH> &lhs, const BoardRepSub<WIDTH2> &rhs) {
        if (WIDTH != WIDTH2) return false;
        return lhs.coord == rhs.coord && lhs.boardRep == rhs.boardRep;
    }

    template<int WIDTH2>
    friend bool operator!=(const BoardRepSub<WIDTH> &lhs, const BoardRepSub<WIDTH2> &rhs) {
        return !(lhs == rhs);
    }

    template<int WIDTH2>
    friend bool operator<(const BoardRepSub<WIDTH> &a, const BoardRepSub<WIDTH2>& b) {
        if (WIDTH != WIDTH2) return WIDTH < WIDTH2;
        if (a.coord != b.coord) return a.coord < b.coord;
        return a.boardRep < b.boardRep;
    }

    static int getByteEstimate() {
        return BoardRepTempl<WIDTH>::getByteEstimate() + 1;
    }
};
