#pragma once
#include <vector>
#include <unordered_set>
#include <string>

struct StateMachine {
    std::vector<int> out, f;
    std::vector<std::vector<int>> g;
    int state;

    StateMachine(std::vector<std::vector<int>> &&g, std::vector<int> &&out, std::vector<int> &&f);
    
    void undoMove(int prev);
    bool canMove(int i);
    int applyMove(int i);

    // precompute
    void calcAndSaveNextState(int state, int i);

private:
    int findNextState(int state, int i);
    int findNextStateUnsafe(int state, int i);
};

StateMachine BuildFSMFromStrings(const std::unordered_set<std::string> &strings);