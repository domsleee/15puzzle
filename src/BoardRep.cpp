#include "../include/BoardRep.h"
#include "../include/Util.h"
#include <cmath>

BoardRep::BoardRep(const BoardRaw &board) {
    boardSize = board.grid.size();
    grid = std::make_unique<uint8_t[]>(getArraySize());
    if (board.grid.size() > 1024) {
        assertm(0, "BoardRep: too big for this data type");
    }
    for (auto i = 0; i < getArraySize(); ++i) grid[i] = 0;

    auto bitsPerTile = getNumBitsPerTile(boardSize);
    auto ind = 0;
    auto remBitsInInd = 8;

    for (auto i = 0; i < boardSize; ++i) {
        auto v = i == board.getBlankTile() ? 0 : board.grid[i];
        //DEBUG(ind << ": " << remBitsInInd << " (i: " << i << ")");

        if (remBitsInInd == bitsPerTile) {
            grid[ind] |= v;
            ind++;
            remBitsInInd = 8;
        } else if (remBitsInInd > bitsPerTile) {
            remBitsInInd -= bitsPerTile;
            grid[ind] |= v << remBitsInInd;
        } else if (remBitsInInd < bitsPerTile) {
            auto bitsInNext = bitsPerTile - remBitsInInd;
            grid[ind] |= (v >> bitsInNext) & getBitmask(bitsPerTile - bitsInNext);
            grid[++ind] |= v << (8 - bitsInNext);
            remBitsInInd = 8 - bitsInNext;
        }
    }
}

BoardRep::BoardRep(const BoardRep &other) {
    grid = std::make_unique<uint8_t[]>(other.getArraySize());
    boardSize = other.boardSize;
    for (auto i = 0; i < getArraySize(); ++i) grid[i] = other.grid[i];
}

BoardRaw BoardRep::toBoard() const {
    auto srt = sqrt(boardSize);
    auto newVec = std::vector<int>(boardSize, 0);

    auto bitsPerTile = getNumBitsPerTile(boardSize);
    auto bitmask = getBitmask(bitsPerTile);
    auto ind = 0;
    auto remBitsInInd = 8;

    for (auto i = 0; i < boardSize; ++i) {
        auto v = 0;
        if (remBitsInInd == bitsPerTile) {
            v = grid[ind];
            ind ++;
            remBitsInInd = 8;
        } else if (remBitsInInd > bitsPerTile) {
            remBitsInInd -= bitsPerTile;
            v = grid[ind] >> remBitsInInd;
        } else if (remBitsInInd < bitsPerTile) {
            auto bitsInNext = bitsPerTile - remBitsInInd;
            v |= grid[ind] << bitsInNext;
            v |= ((grid[++ind] >> (8 - bitsInNext)) & getBitmask(bitsInNext));
            remBitsInInd = 8 - bitsInNext;
        }
        v &= bitmask;
        newVec[i] = v;
    }
    return BoardRaw(newVec, srt, srt);
}
