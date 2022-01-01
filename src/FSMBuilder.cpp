#include "../include/FSMBuilder.h"
#include "../include/ForbiddenWords.h"
#include "../include/ForbiddenWordsFast.h"
#include "../include/Util.h"
#include "../include/InputParser.h"

#include <stack>
#include <queue>
#include <unordered_set>

StateMachineSimple dfsOrderFSM(StateMachine fsm);

FSMBuilder::FSMBuilder(int width, int height, int maxDepth):
    width(width),
    height(height),
    maxDepth(maxDepth)
    {}

StateMachineSimple FSMBuilder::build() {
    START_TIMER(forbiddenWords);
    //auto forbiddenWords = ForbiddenWords(14, width, height);
    auto forbiddenWords = ForbiddenWordsFast(maxDepth, width, height);
    forbiddenWords.printMessage();
    auto strings = forbiddenWords.getForbiddenWords();
    DEBUG("found " << strings.size() << " forbidden strings");
    END_TIMER(forbiddenWords);

    DEBUG("Building FSM from " << strings.size() << " strings");
    START_TIMER(FSM);
    auto fsm = BuildFSMFromStrings(strings);
    END_TIMER(FSM);

    DEBUG("re-order FSM");
    START_TIMER(FSM2);
    auto fsm2 = dfsOrderFSM(fsm);
    END_TIMER(FSM2);

    return fsm2;
}


StateMachineSimple dfsOrderFSM(StateMachine fsm) {

    // out, g
    std::queue<int> st;
    std::unordered_set<int> seen;
    std::vector<int> dfsOrder;

    st.push(0);
    seen.insert(0);

    while (!st.empty()) {
        auto t = st.front(); st.pop();
        dfsOrder.push_back(t);

        for (int i = 0; i < 4; ++i) {
            fsm.undoMove(t);
            if (!fsm.canMove(i)) continue;
            fsm.applyMove(i);
            auto nxState = fsm.state;
            if (seen.count(nxState)) continue;
            seen.insert(nxState);
            st.push(nxState);
        }
    }
    auto newNumStates = dfsOrder.size();

    int numStates = fsm.states;
    std::vector<int> out(newNumStates, 0), f(newNumStates, -1);
    std::vector<std::array<int, 4>> g(newNumStates, std::array<int, 4>());
    //DEBUG("DFS ORDER " << dfsOrder.size() << ", numStates:" << numStates);
    //assertm(dfsOrder.size() == numStates, "dfs");
    std::unordered_set<int> prunedNodes = {};
    for (auto i = 0; i < numStates; ++i) prunedNodes.insert(i);
    for (auto node: dfsOrder) prunedNodes.erase(node);

    // dfsOrder = [1, 3, 0, 2]
    // dfsOrderInv = [2, 0, 3, 1]
    for (auto i = 0; i < newNumStates; ++i) {
        out[i] = fsm.outAndG[i].first;
        for (int j = 0; j < 4; ++j) g[i][j] = fsm.outAndG[dfsOrder[i]].second[j];
    }

    std::vector<int> dfsOrderInv(numStates);
    for (int i = 0; i < newNumStates; ++i) {
        dfsOrderInv[dfsOrder[i]] = i;
    }

    for (int i = 0; i < newNumStates; ++i) {
        for (int j = 0; j < 4; ++j) g[i][j] = fsm.outAndG[dfsOrder[i]].second[j];
        out[i] = fsm.outAndG[dfsOrder[i]].first;
    }

    for (int i = 0; i < newNumStates; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (prunedNodes.count(g[i][j])) {
                g[i][j] = WORD_STATE;
            } else g[i][j] = dfsOrderInv[g[i][j]];
        }
    }
    // 59...
    DEBUG("Rebuilt with " << newNumStates << " STATES");
    //exit(0);
    return StateMachineSimple(std::move(g));
}
