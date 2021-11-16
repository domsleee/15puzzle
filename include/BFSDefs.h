#include "BoardRaw.h"
#include "Util.h"

struct BFSNode {
    BoardRaw board;
    int dist;
    std::string path;
    int dir;

    BFSNode(const BoardRaw &board, int dist, std::string path, int dir)
        : board(board),
          dist(dist),
          path(path),
          dir(dir) {}
};

std::unique_ptr<BoardRaw> getBoardFromString(BoardRaw startBoard, std::string str);
