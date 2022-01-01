#ifndef IDASTAR_MULTI_INITIALNODES_H
#define IDASTAR_MULTI_INITIALNODES_H

#include "AhoCorasick.h"
#include "Direction.h"
#include <vector>

template <class B>
class IdastarMultiInitialNodes {
    StateMachine &fsm;

public:
    IdastarMultiInitialNodes(StateMachine &fsm);
    struct InitialNode {
        InitialNode(B startBoard, int g, int fsmState, const std::vector<Direction> &path);
        B startBoard;
        int g, fsmState;
        std::vector<Direction> path;
    };

    std::vector<InitialNode> getInitialNodes(const B& start, int targetNodes);
    std::vector<InitialNode> getInitialNodes2(const B& start, int targetNodes);
};



#endif
