#include "../include/BFSDefs.h"

std::unique_ptr<BoardRaw> getBoardFromString(BoardRaw startBoard, std::string str) {
    for (auto s: str) {
        auto d = charToDirection(s);
        if (startBoard.canMove(d)) {
            startBoard.applyMove(d);
        } else {
            return nullptr;
        }
    }
    return std::make_unique<BoardRaw>(startBoard);
}
