#include "../include/BoardRepTempl.h"
#include "../include/Util.h"
#include <cmath>
#include <bitset>

template <int WIDTH>
BoardRepTempl<WIDTH>::BoardRepTempl(const BoardRaw &board) {
    if (board.grid.size() != WIDTH*WIDTH) {
        DEBUG("board.grid.size(): " << board.grid.size() << ", WIDTH*WIDTH: " << WIDTH*WIDTH);
        assertm(0, "BoardRepTempl: incorrect type");
    }
    for (std::size_t i = 0; i < grid.size(); ++i) grid[i] = 0;

    auto bitsPerTile = getNumBitsPerTile(WIDTH*WIDTH);
    auto ind = 0;
    auto remBitsInInd = 8;

    for (auto i = 0; i < WIDTH*WIDTH; ++i) {
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

template <int WIDTH>
BoardRepTempl<WIDTH>::BoardRepTempl(const BoardRaw &board, uint8_t coord) {
    
    assertm((coord % board.getWidth()) + WIDTH - 1 < board.getWidth(), "coord will not fit horizontally");
    assertm((coord / board.getWidth()) + WIDTH - 1 < board.getWidth(), "coord will not fit vertically");

    for (std::size_t i = 0; i < grid.size(); ++i) grid[i] = 0;

    auto bitsPerTile = getNumBitsPerTile(WIDTH*WIDTH);
    auto ind = 0;
    auto remBitsInInd = 8;

    //DEBUG("BEGIN");
    auto i = coord;
    for (auto j = 0; j < WIDTH; ++j, i += board.getWidth() - WIDTH) {
        for (auto k = 0; k < WIDTH; k++, ++i) {
            //DEBUG("column: " << i % board.getWidth());
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
}

template <int WIDTH>
BoardRaw BoardRepTempl<WIDTH>::toBoard() const {
    auto newVec = std::vector<int>(WIDTH*WIDTH, 0);

    auto bitsPerTile = getNumBitsPerTile(WIDTH*WIDTH);
    auto bitmask = getBitmask(bitsPerTile);
    auto ind = 0;
    auto remBitsInInd = 8;

    for (auto i = 0; i < WIDTH*WIDTH; ++i) {
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
    return BoardRaw(newVec, WIDTH, WIDTH);
}


template class BoardRepTempl<3>;
template class BoardRepTempl<4>;
template class BoardRepTempl<5>;
template class BoardRepTempl<6>;
template class BoardRepTempl<7>;
template class BoardRepTempl<8>;
template class BoardRepTempl<9>;

