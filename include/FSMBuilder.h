#include "../include/AhoCorasick.h"

class FSMBuilder {
    int width, height;
public:
    FSMBuilder(int width, int height);
    StateMachine build();
};

