#include "../include/AhoCorasick.h"
#include "StateMachineSimple.h"

class FSMBuilder {
    int width, height, maxDepth;
public:
    FSMBuilder(int width, int height, int maxDepth);
    StateMachineSimple build() const;
    std::vector<std::string> getForbiddenWords() const;
};

StateMachineSimple dfsOrderFSM(StateMachine fsm);
