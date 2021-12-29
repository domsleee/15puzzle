#ifndef IDASTAR_H
#define IDASTAR_H

#include <vector>

#include "Direction.h"
#include "AhoCorasick.h"
#include "StateMachineSimple.h"

template <class B>
class Idastar {
private:
    std::vector<Direction> path;
    int minCost;
    int limit;
    long long nodes;
    StateMachineSimple &fsm;

public:
    Idastar(StateMachineSimple &fsm);

    std::vector<Direction> solve(const B& start);
    bool dfs(B& root, int g);
};

#endif
