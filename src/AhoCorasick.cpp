#include <queue>
#include "../include/AhoCorasick.h"
#include "../include/Util.h"

#define AHO_DEBUG(x)

StateMachine::StateMachine(std::vector<std::vector<int>> &&g, std::vector<int> &&out, std::vector<int> &&f)
    : g(g),
      out(out),
      f(f),
      state(0) {}

int StateMachine::applyMove(int i) {
    auto oldState = state;
    auto answer = getAnsUsingFailFn(state, i);
    // auto answer = state;  
    // while (g[answer][i] == -1)
    //     answer = getAnsUsingFailFn(answer, i);//f[answer];
    // //auto answer2 = getAnsUsingFailFn(state, i);
  
    //assert(answer == answer2);
    state = g[answer][i];
    return oldState;
}

int StateMachine::getAnsUsingFailFn(int answer, int i) {
    if (g[answer][i] == -1) {
        return f[answer] = getAnsUsingFailFn(f[answer], i);
    }
    return answer;
}

int StateMachine::applyMoveOld(int i) {
    int oldState = state;
    state = g[state][i];
    //DEBUG("fsm: " << oldState << " => " << state);
    return oldState;
}

void StateMachine::undoMove(int state) {
    this->state = state;
}

bool StateMachine::canMove(int i) {
    return !out[g[state][i]];
}

StateMachine BuildFSMFromStrings(const std::unordered_set<std::string> &strings) {
    auto numNodes = 1;
    for (auto s: strings) numNodes += s.size();

    std::vector<int> out(numNodes, 0), f(numNodes, -1);
    std::vector<std::vector<int>> g(numNodes, std::vector<int>(4, -1));
    auto states = 1;

    auto i = 0;
    for (auto string: strings) {
        auto currentState = 0;
        for (char c: string) {
            auto index = static_cast<int>(charToDirection(c));
            if (g[currentState][index] == -1) g[currentState][index] = states++;
            currentState = g[currentState][index];
            AHO_DEBUG(string << "[] = " << currentState);
        }

        out[currentState] |= 1;
        i++;
    }
    for (auto i = 0; i < 4; ++i) {
        if (g[0][i] == -1) g[0][i] = 0;
    }

    std::queue<int> q;
    for (auto i = 0; i < 4; ++i) {
        if (g[0][i] != 0) {
            f[g[0][i]] = 0;
            q.push(g[0][i]);
        }
    }

    while (q.size()) {
        auto state = q.front(); q.pop();
        for (auto i = 0; i < 4; ++i) {
            if (g[state][i] == -1) continue;
            auto failure = f[state];
            while (g[failure][i] == -1) failure = f[failure];

            failure = g[failure][i] > 0;
            f[g[state][i]] = failure;

            out[g[state][i]] |= out[failure];
            q.push(g[state][i]);
        }
    }

    for (auto i = 0; i < out.size(); ++i) {
        AHO_DEBUG("out[" << i << "] = " << out[i]);
    }
    for (auto i = 0; i < out.size(); ++i) {
        AHO_DEBUG("f[" << i << "] = " << f[i]);
    }

    q.push(0);
    while (q.size()) {
        auto state = q.front(); q.pop();
        for (auto i = 0; i < 4; ++i) {
            if (g[state][i] == -1) {
                auto relState = state;
                //while (g[relState][i] == -1) relState = f[relState];
                //g[state][i] = g[relState][i];
            }
            else if (g[state][i] != 0) q.push(g[state][i]);
        }
    }

    return StateMachine(std::move(g), std::move(out), std::move(f));
}