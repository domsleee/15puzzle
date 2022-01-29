#include <vector>

#include "StateMachineSimple.h"
#include "Util.h"
#include "BoardRep.h"
#include "../include/AhoCorasick.h"

using SM = StateMachine;

struct DFSCountResult {
    DFSCountResult(int width, int height)
        : counts(width*height, 0),
          totalNodes(0),
          probs(width*height, 0.00) {}
    std::vector<long long> counts;
    long long totalNodes;
    std::vector<double> probs;
};

std::vector<double> getTileProbability(SM &fsm, int width, int height) ;