#include "../include/TileProbability.h"

#include <stack>

void dfs(DFSCountResult& res, BoardRaw &board, SM &fsm, int g, int limit) {
    res.counts[board.getBlankTile()]++;
    res.totalNodes++;

    if (g == limit) return;

    for (auto i = 0; i < 4; ++i) {
        auto dir = static_cast<Direction>(i);
        if (board.canMove(dir) && fsm.canMove(i)) {
            auto prevMove = board.applyMove(dir);
            auto prevFSM = fsm.applyMove(i);

            dfs(res, board, fsm, g+1, limit);

            fsm.undoMove(prevFSM);
            board.undoMove(prevMove);
        }
    }
}

std::vector<double> getTileProbability(SM &fsm, int width, int height) {
    std::vector<DFSCountResult> results = {};
    auto startBoards = getAllStartingBoards(width, height);
    auto maxDepth = fsm.outAndG.size() == 1 ? 14 : 17;

    for (auto &startBoard: startBoards) {
        auto res = DFSCountResult(width, height);

        std::vector<int> numNodes(1, 0);
        fsm.undoMove(0);
        for (auto depth = 1; depth <= maxDepth; ++depth) {
            dfs(res, startBoard, fsm, 0, depth);
            numNodes.resize(depth+1);
            numNodes[depth] = res.totalNodes;
            auto ratio = 0.00;
            if (depth > 1) ratio = (double)numNodes[depth] / numNodes[depth-1];
            //DEBUG(depth << ", " << res.totalNodes << " (" << ratio << ")");
        }

        results.push_back(res);
    }

    std::vector<double> probs(width * height, 0.00);
    auto length = width * height;
    for (auto i = 0; i < length; ++i) {
        double probsSum = 0.00;
        for (auto &res: results) {
            res.probs[i] = (double)res.counts[i] / res.totalNodes;
            probsSum += res.probs[i];
        }
        probs[i] = probsSum / (width * height);
    }

    for (auto r = 0; r < height; ++r) {
        std::cout << "  ";
        for (auto c = 0; c < width; ++c) {
            std::cout << probs[r*width + c];
            if (c < width-1) std::cout << ", ";
        }
        std::cout << "\n";
    }

    return probs;
}