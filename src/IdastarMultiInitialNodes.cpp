
#include "../include/IdastarMultiInitialNodes.h"
#include "../include/Board.h"
#include "../include/BoardRect.h"
#include "../include/Util.h"

#include <queue>
#include <unordered_map>

template <class B>
IdastarMultiInitialNodes<B>::InitialNode::InitialNode(B startBoard, int g, int fsmState, const std::vector<Direction> &path)
    : startBoard(startBoard),
      g(g),
      fsmState(fsmState),
      path(path) {}

template <class B>
IdastarMultiInitialNodes<B>::IdastarMultiInitialNodes(StateMachineSimple &fsm)
    : fsm(fsm)
    {}

template <class B>
struct BFSNodeWithNumSucc {
    int fsmState;
    B node;
    std::vector<Direction> succ;
    int g;

    BFSNodeWithNumSucc(StateMachineSimple &fsm, int fsmState, const B &node, int g)
        : fsmState(fsmState),
          node(node),
          g(g) {

        fsm.undoMove(fsmState);

        for (auto i = 0; i < 4; ++i) {
            auto dir = static_cast<Direction>(i);
            if (fsm.canMove(i) && node.canMove(dir)) {
                succ.push_back(dir);
            }
        }
    }
};

std::string gridHash(const std::vector<int> &grid) {
    std::string res = "";
    for (auto i: grid) res.push_back('0' + i);
    return res;
}

template <class B>
std::vector<typename IdastarMultiInitialNodes<B>::InitialNode> IdastarMultiInitialNodes<B>::getInitialNodes2(const B& start, const std::vector<int>& solution, int targetNodes) {
    std::unordered_set<std::string> seen;
    std::unordered_map<std::string, std::pair<Direction, std::string>> pred;

    std::queue<BFSNodeWithNumSucc<B>> q;
    q.push({fsm, 0, start, 0});

    auto solvedHash = gridHash(solution);

    while (!q.empty()) {
        auto top = q.front();
        //DEBUG(q.size() - 1 + top.succ.size() << " > " << targetNodes << " ?");
        if ((int)q.size() - 1 + (int)top.succ.size() > targetNodes) {
            break;
        }

        q.pop();
        auto currHash = gridHash(top.node.getGrid());
        //DEBUG("eval board " << currHash);

        bool doBreak = false;
        for (auto dir: top.succ) {
            auto newNode = top.node;
            newNode.applyMove(dir);

            auto boardStr = gridHash(newNode.getGrid());

            if (seen.count(boardStr)) continue;
            if (boardStr == solvedHash) {
                doBreak = true;
            };
            seen.insert(boardStr);
            pred[boardStr] = {dir, currHash};

            fsm.undoMove(top.fsmState);
            fsm.applyMove(static_cast<int>(dir));
            q.push({fsm, fsm.state, newNode, top.g + 1});
        }

        if (doBreak) break;
    }

    std::vector<InitialNode> res;
    while (!q.empty()) {
        auto top = q.front(); q.pop();
        std::vector<Direction> path;
        auto node = top.node;
        auto nodeHash = gridHash(node.getGrid());
        while (true) {
            if (pred.count(nodeHash) == 0) break;
            auto [predDir, predHash] = pred[nodeHash];
            path.push_back(predDir);
            nodeHash = predHash;
        }
        res.push_back({top.node, top.g, top.fsmState, path});
    }

    return res;
}

template class IdastarMultiInitialNodes<Board>;
template class IdastarMultiInitialNodes<BoardRect>;
