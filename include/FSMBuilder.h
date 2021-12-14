#include "../include/AhoCorasick.h"

class FSMBuilder {
    int width, height, maxDepth;
public:
    FSMBuilder(int width, int height, int maxDepth);
    StateMachine build();
};

