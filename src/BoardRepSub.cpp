#include "../include/BoardRepSub.h"

template <int WIDTH>
BoardRepSub<WIDTH>::BoardRepSub(const BoardRaw &board, uint8_t coord)
    : coord(coord),
      boardRep(BoardRepTempl<WIDTH>(board, coord)) 
{
}


template class BoardRepSub<3>;
template class BoardRepSub<4>;
template class BoardRepSub<5>;
template class BoardRepSub<6>;

