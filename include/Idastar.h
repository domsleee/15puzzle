#ifndef IDASTAR_H
#define IDASTAR_H

#include <vector>

#include "Direction.h"
#include "AhoCorasick.h"

template <class B>
class Idastar {
private:
    int minCost;
    int limit;
    long long nodes;
    StateMachine &fsm;

public:
    std::vector<Direction> path;

    Idastar(StateMachine &fsm);

    std::vector<Direction> solve(const B& start);
    bool dfs(B& root, int g);
    void clearPathAndSetLimit(int limit);

    long long getNodes();
    int getMinCost();
};

#endif
