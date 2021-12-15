#include "../include/FSMBuilder.h"
#include "../include/ForbiddenWords.h"
#include "../include/Util.h"

#include <stack>
#include <unordered_set>

FSMBuilder::FSMBuilder(int width, int height, int maxDepth):
    width(width),
    height(height),
    maxDepth(maxDepth)
    {}


StateMachine dfsOrderFSM(StateMachine &fsm) {
    int numStates = fsm.states;
    std::vector<int> out(numStates, 0), f(numStates, -1);
    std::vector<std::vector<int>> g(numStates, std::vector<int>(4, -1));

    // out, g
    std::stack<int> st;
    std::unordered_set<int> seen;
    std::vector<int> dfsOrder;

    st.push(0);
    seen.insert(0);
    dfsOrder.push_back(0);

    while (!st.empty()) {
        auto t = st.top(); st.pop();
    
        for (int i = 0; i < 4; ++i) {
            //if (!fsm.canMove(i)) continue;
            fsm.undoMove(t);
            fsm.applyMove(i);
            auto nxState = fsm.state;
            if (seen.count(nxState)) continue;
            seen.insert(nxState);
            st.push(nxState);
            dfsOrder.push_back(nxState);
        }
    }

    DEBUG(dfsOrder.size() << " VS " << numStates);
    assertm(dfsOrder.size() == numStates, "dfs");

    // dfsOrder = [1, 3, 0, 2]
    // dfsOrderInv = [2, 0, 3, 1]
    std::vector<int> dfsOrderInv(numStates);
    for (int i = 0; i < numStates; ++i) {
        dfsOrderInv[dfsOrder[i]] = i;
    }

    for (int i = 0; i < numStates; ++i) {
        g[i] = g[dfsOrderInv[i]];
    }

    for (int i = 0; i < numStates; ++i) {
        out[i] = dfsOrderInv[out[i]];
        for (int j = 0; j < 4; ++j) {
            g[i][j] = dfsOrderInv[g[i][j]];
        }
    }
    return StateMachine(std::move(g), std::move(out), std::move(f), numStates);
}

StateMachine FSMBuilder::build() {
    DEBUG("Getting forbidden words with depth: " << maxDepth);
    START_TIMER(forbiddenWords);
    auto forbiddenWords = ForbiddenWords(maxDepth, width, height);
    auto strings = forbiddenWords.getForbiddenWords();
    DEBUG("found " << strings.size() << " forbidden strings");
    END_TIMER(forbiddenWords);

    DEBUG("Building FSM from strings");
    START_TIMER(FSM);
    auto fsm = BuildFSMFromStrings(strings);
    END_TIMER(FSM);

    DEBUG("re-order FSM");
    START_TIMER(FSM2);
    auto fsm2 = dfsOrderFSM(fsm);
    END_TIMER(FSM2);

    return fsm2;
}
