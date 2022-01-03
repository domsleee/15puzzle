#ifndef IDASTAR_MULTI_INITIALNODES_H
#define IDASTAR_MULTI_INITIALNODES_H

#include "AhoCorasick.h"
#include "Direction.h"
#include "StateMachineSimple.h"
#include <vector>

template <class B>
class IdastarMultiInitialNodes {
    StateMachineSimple &fsm;

public:
    IdastarMultiInitialNodes(StateMachineSimple &fsm);
    struct InitialNode {
        InitialNode(B startBoard, int g, int fsmState, const std::vector<Direction> &path);
        B startBoard;
        int g, fsmState;
        std::vector<Direction> path;
    };

    std::vector<InitialNode> getInitialNodes2(const B& start, const std::vector<int> &solution, int targetNodes);
};



#endif
