#include "../include/BranchFactor.h"

#include <queue>
#include <unordered_map>

void evaluateBranchFactor(StateMachineSimple &fsm, int width, int height) {
    auto startBoard = getAllStartingBoards(width, height)[0];

    std::queue<std::pair<int, BoardRaw>> q;
    std::unordered_map<BoardRep, int> dist;
    std::vector<int> numNodes;
    q.push({0, startBoard});
    auto startBoardRep = BoardRep(startBoard);
    dist[startBoardRep] = 0;
    auto knownDist = 0, nodeCount = 0;
    fsm.undoMove(0);

    while (!q.empty()) {
        auto [fsmState, board] = q.front(); q.pop();
        auto boardRep = BoardRep(board);
        auto myDist = dist.at(boardRep);
        if (myDist > knownDist) {
            knownDist = myDist;
            auto ratio = 0.00;
            if (numNodes.size() > 0) ratio = (double)nodeCount / numNodes[numNodes.size()-1];
            DEBUG(knownDist << ", " << nodeCount << " (" << ratio << ")");
            numNodes.push_back(nodeCount);
            nodeCount = 0;
        }

        nodeCount++;
        fsm.undoMove(fsmState);
        for (auto i = 0; i < 4; ++i) {
            auto dir = static_cast<Direction>(i);
            if (board.canMove(dir) && fsm.canMove(i)) {
                auto prevMove = board.applyMove(dir);
                auto prevFSM = fsm.applyMove(i);
                auto newBoardRep = BoardRep(board);

                //if (!dist.count(newBoardRep)) {
                    dist[newBoardRep] = myDist + 1;
                    q.push({fsm.state, board});
                //}

                fsm.undoMove(prevFSM);
                board.undoMove(prevMove);
            }
        }
    }
}