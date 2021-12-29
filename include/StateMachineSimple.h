#pragma once
#include <vector>
#include <array>
#include "Util.h"

const int WORD_STATE = -2;

struct StateMachineSimple {
    std::vector<std::array<int, 4>> g;
    int state;
    StateMachineSimple(const std::vector<std::array<int, 4>> &&g);

    int applyMove(int i);
    void undoMove(int prev);
    bool canMove(int i) const;
};


inline int StateMachineSimple::applyMove(int i) {
    auto oldState = state;
    state = g[state][i];
    return oldState;
}

inline void StateMachineSimple::undoMove(int state) {
    this->state = state;
}

inline bool StateMachineSimple::canMove(int i) const {
    //if (g[state][i] == WORD_STATE) { DEBUG("HELLO??"); exit(0); }
    return g[state][i] != WORD_STATE;
}
