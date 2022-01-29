#include "../include/BoardRepSub.h"

template <int WIDTH>
BoardRepSub<WIDTH>::BoardRepSub(const BoardRaw &board, uint8_t coord)
    : coord(coord),
      boardRep(BoardRepTempl<WIDTH>(board, coord)) 
{
}
