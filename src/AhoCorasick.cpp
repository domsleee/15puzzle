#include <queue>
#include "../include/AhoCorasick.h"
#include "../include/Util.h"

#define AHO_DEBUG(x) DEBUG(x)
#define SM_DEBUG(x)

StateMachine::StateMachine(std::vector<std::vector<int>> &&g, std::vector<int> &&out, std::vector<int> &&f, int states)
    : g(g),
      out(out),
      f(f),
      state(0),
      states(states) {}

int StateMachine::applyMove(int i) {
    auto oldState = state;
    state = findNextStateUnsafe(state, i);
    SM_DEBUG("apply move " << oldState << " => " << state);
    return oldState;
}

void StateMachine::undoMove(int state) {
    this->state = state;
    SM_DEBUG("undo move " << state);
}

bool StateMachine::canMove(int i) const {
    auto nextState = findNextStateUnsafe(state, i);
    return !out[nextState];
}

int StateMachine::findNextStateUnsafe(int state, int i) const {
    return g[state][i];
}

void StateMachine::calcAndSaveNextState(int state, int i) {
    if (g[state][i] != -1) return;
    auto answer = state;  
    while (g[answer][i] == -1)
        answer = f[answer];
    g[state][i] = g[answer][i];
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
            //AHO_DEBUG(string << "[] = " << currentState);
        }

        out[currentState] |= 1;
        i++;
    }
    AHO_DEBUG("STATES: " << states << ", numNodes " << numNodes);
    out.resize(states);
    f.resize(states);
    g.resize(states);
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

            failure = g[failure][i];
            f[g[state][i]] = failure;

            out[g[state][i]] |= out[failure];
            q.push(g[state][i]);
        }
    }

    for (auto i = 0; i < states; ++i) {
        //AHO_DEBUG("out[" << i << "] = " << out[i]);
    }
    for (auto i = 0; i < states; ++i) {
        //AHO_DEBUG("f[" << i << "] = " << f[i]);
    }
    AHO_DEBUG("Precomputing all states for FSM...");

    auto fsm = StateMachine(std::move(g), std::move(out), std::move(f), states);
    for (int state = 0; state < states; ++state) {
        for (int i = 0; i < 4; ++i) {
            fsm.calcAndSaveNextState(state, i);
        }
    }
    return fsm;
}