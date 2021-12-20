#include "../include/BoardRep.h"
#include <cmath>

BoardRep::BoardRep(const BoardRaw &board) {
    grid.resize(board.grid.size(), 0);
    if (grid.size() > 255) {
        assertm(0, "BoardRep: too big for this data type");
    }
    for (auto i = 0; i < board.grid.size(); ++i) {
        grid[i] = i == board.getBlankTile() ? 0 : board.grid[i];
    }
}

BoardRaw BoardRep::toBoard() const {
    auto srt = sqrt(grid.size());
    auto newVec = std::vector<int>(grid.begin(), grid.end());
    return BoardRaw(newVec, srt, srt);
}
