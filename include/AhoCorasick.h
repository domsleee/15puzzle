#pragma once
#include <vector>
#include <unordered_set>
#include <string>

#define AHO_DEBUG(x)
#define G(i) outAndG[i].second

struct StateMachine {
    std::vector<int> f;
    int state, states;
    std::vector<std::pair<int, std::vector<int>>> outAndG;

    StateMachine(std::vector<std::vector<int>> &&g, std::vector<int> &&out, std::vector<int> &&f, int states);

    int applyMove(int i);
    void undoMove(int prev);
    bool canMove(int i) const;

    // precompute
    void calcAndSaveNextState(int state, int i);

private:
    int findNextStateUnsafe(int state, int i) const;
};

StateMachine BuildFSMFromStrings(const std::unordered_set<std::string> &strings);

inline int StateMachine::applyMove(int i) {
    auto oldState = state;
    state = findNextStateUnsafe(state, i);
    AHO_DEBUG("apply move " << oldState << " => " << state);
    return oldState;
}

inline void StateMachine::undoMove(int state) {
    this->state = state;
    AHO_DEBUG("undo move " << state);
}

inline bool StateMachine::canMove(int i) const {
    auto nextState = findNextStateUnsafe(state, i);
    return !outAndG[nextState].first;
}

inline int StateMachine::findNextStateUnsafe(int state, int i) const {
    return G(state)[i];
}