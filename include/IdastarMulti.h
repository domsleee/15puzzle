#ifndef IDASTARMULTI_H
#define IDASTARMULTI_H

#include "Direction.h"
#include "AhoCorasick.h"
#include "IdastarMultiInitialNodes.h"
#include <vector>

template <class B>
class IdastarMulti {
private:
    StateMachine &fsm;
    int limit, minCost;
    long long nodes;

    int numWorkers;
    std::vector<std::vector<int>> serverReadPipes, serverWritePipes;

    struct InitialNode {
        InitialNode(B startBoard, int g, int fsmState);
        B startBoard;
        int g, fsmState;
    };

    std::vector<InitialNode> getInitialNodes(const B& start);
    void doClient(int nodeId, std::vector<typename IdastarMultiInitialNodes<B>::InitialNode> initialNodes);
    void writeAll(const void* ptr, size_t size);
    void setupWorkers(int numWorkers);


public:
    IdastarMulti(StateMachine &fsm);

    std::vector<Direction> solve(const B& start);
};

#endif
