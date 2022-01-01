#include "../include/Idastar.h"

#include "../include/Board.h"
#include "../include/BoardRect.h"
#include "../include/Util.h"

constexpr int INF = 1000;

template <class B>
Idastar<B>::Idastar(StateMachineSimple &fsm) : path({}), minCost(INF), limit(0), nodes(0), fsm(fsm) {}

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

    while (path.empty()) {
        minCost = INF;
        DEBUG(' ' << limit << ", " << nodes);
        auto copy = start;
        fsm.undoMove(0);
        if (dfs(copy, 0)) {
            DEBUG("Nodes expanded: " << nodes);
            return path;
        }

        limit = minCost;
    }

    return path;
}

template <class B>
bool Idastar<B>::dfs(B& node, int g) {
    auto h = node.getHeuristic();
    auto f = g + h;

    //DEBUG_WITH_PID("f: " << f << ", limit: " << limit);

    if (h == 0) [[unlikely]] {
        // Found goal state (heuristic = 0)
        return true;
    } else if (f > limit) {
        // Exceeded search depth, store next smallest depth
        if (f < minCost) {
            minCost = f;
        }
        return false;
    }

    nodes += 1;
    auto prevFsm = -1;
    typename B::MoveState prev;

    for (int i = 0; i < 4; ++i) {
        auto dir = static_cast<Direction>(i);
        if (fsm.canMove(i) && node.canMove(dir)) {
            prev = node.applyMove(dir);
            prevFsm = fsm.applyMove(i);

            if (dfs(node, g + 1)) {
                path.push_back(dir);
                return true;
            }

            fsm.undoMove(prevFsm);
            node.undoMove(prev);
        }
    }

    return false;
}

template <class B>
void Idastar<B>::clearPathAndSetLimit(int limit) {
    this->limit = limit;
    minCost = INF;
    path.clear();
}

template <class B>
long long Idastar<B>::getNodes() { return nodes; }

template <class B>
int Idastar<B>::getMinCost() { return minCost; }

template class Idastar<Board>;
template class Idastar<BoardRect>;