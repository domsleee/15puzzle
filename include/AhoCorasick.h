#pragma once
#include <vector>
#include <unordered_set>
#include <string>

struct StateMachine {
    std::vector<std::vector<int>> g;
    std::vector<int> out, f;
    int state;

    StateMachine(std::vector<std::vector<int>> &&g, std::vector<int> &&out, std::vector<int> &&f);

    int applyMove(int i);
    void undoMove(int prev);
    bool canMove(int i) const;

    // precompute
    void calcAndSaveNextState(int state, int i);

private:
    int findNextStateUnsafe(int state, int i) const;
};

StateMachine BuildFSMFromStrings(const std::unordered_set<std::string> &strings);