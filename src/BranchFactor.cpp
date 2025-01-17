#include "../include/BranchFactor.h"
#include "../include/Util.h"

#include <stack>
#include <unordered_map>

void dfs(BoardRaw &board, StateMachineSimple &fsm, int g, int limit, int &nodeCount) {
    if (g == limit) return;
    nodeCount++;

    for (auto i = 0; i < 4; ++i) {
        auto dir = static_cast<Direction>(i);
        if (board.canMove(dir) && fsm.canMove(i)) {
            auto prevMove = board.applyMove(dir);
            auto prevFSM = fsm.applyMove(i);

            dfs(board, fsm, g+1, limit, nodeCount);

            fsm.undoMove(prevFSM);
            board.undoMove(prevMove);
        }
    }
}

void evaluateBranchFactor(StateMachineSimple &fsm, int width, int height) {
    auto startBoard = getAllStartingBoards(width, height)[0];

    std::vector<int> numNodes(1, 0);
    fsm.undoMove(0);
    for (auto depth = 1; depth <= 25; ++depth) {
        int nodeCount = 0;
        dfs(startBoard, fsm, 0, depth, nodeCount);
        numNodes.resize(depth+1);
        numNodes[depth] = nodeCount;
        auto ratio = 0.00;
        if (depth > 1) ratio = (double)numNodes[depth] / numNodes[depth-1];
        DEBUG(depth << ", " << nodeCount << " (" << ratio << ")");
    }
}