#include "../include/Idastar.h"

#include "../include/Board.h"
#include "../include/BoardRect.h"
#include "../include/Util.h"

constexpr int INF = 1000;

template <class B>
Idastar<B>::Idastar(StateMachine &fsm) : path({}), minCost(INF), limit(0), nodes(0), fsm(fsm) {}

template <class B>
std::vector<Direction> Idastar<B>::solve(const B& start) {
    DEBUG("Running single threaded");
    DEBUG("Solving: \n" << start);

    path.clear();
    nodes = 1;
    limit = start.getHeuristic();

    if (limit == 0) {
        DEBUG("Already solved");
        return path;
    }

    DEBUG("Limit, Nodes:");

    // Starting moves (for prevMove)
    auto startMoves = start.getMoves();

    while (path.empty()) {
        minCost = INF;
        DEBUG(' ' << limit << ", " << nodes);
        //DEBUG("FSM STATE " << fsm.state);
        ///DEBUG("FSM STATE after undo " << fsm.state);
        fsm.undoMove(0);
        //assertm(fsm.state == 0, "fsm state is 0");

        for (auto startDir : startMoves) {
            auto copy = start;
            copy.applyMove(startDir);
            fsm.applyMove(static_cast<int>(startDir));

            if (dfs(copy, 1, inverse(startDir))) {
                path.push_back(startDir);
                DEBUG("Nodes expanded: " << nodes);
                fsm.undoMove(0);
                return path;
            } else fsm.undoMove(0);
        }

        limit = minCost;
    }

    return path;
}

template <class B>
bool Idastar<B>::dfs(B& node, int g, Direction prevMove) {
    auto h = node.getHeuristic();
    auto f = g + h;

    if (h == 0) [[unlikely]] {
            // Found goal state (heuristic = 0)
            return true;
        }
    else if (f > limit) {
        // Exceeded search depth, store next smallest depth
        if (f < minCost) {
            minCost = f;
        }
        return false;
    }

    nodes += 1;

    for (int i = 0; i < 4; i++) {
        auto dir = static_cast<Direction>(i);
        if (fsm.canMove(i) && node.canMove(dir)) {
            auto prev = node.applyMove(dir);
            auto prevFsm = fsm.applyMove(i);

            if (dfs(node, g + 1, inverse(dir))) {
                path.push_back(dir);
                fsm.undoMove(prevFsm);
                return true;
            } else fsm.undoMove(prevFsm);

            node.undoMove(prev);
        }
    }

    return false;
}

template class Idastar<Board>;
template class Idastar<BoardRect>;