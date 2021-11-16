#pragma once
#include <vector>
#include <unordered_set>

struct StateMachine {
    std::vector<int> out, f;
    std::vector<std::vector<int>> g;
    int state;

    StateMachine(std::vector<std::vector<int>> &&g, std::vector<int> &&out, std::vector<int> &&f);
    void undoMove(int prev);
    bool canMove(int i);
    int applyMove(int i);
    int getAnsUsingFailFn(int answer, int i);
    int applyMoveOld(int i);
};

StateMachine BuildFSMFromStrings(const std::unordered_set<std::string> &strings);