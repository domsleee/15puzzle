#include "../include/BranchFactor.h"
#include "../include/Util.h"
#include "../include/AhoCorasick.h"
#include "../include/FSMBuilder.h"

#include <stack>
#include <unordered_map>

void dfs(BoardRaw &board, StateMachineSimple &fsm, int g, int limit, int &nodeCount) {
    if (g == limit) return;
    nodeCount++;

    for (auto i = 0; i < 4; ++i) {
        auto dir = static_cast<Direction>(i);
        if (board.canMove(dir) && fsm.canMove(i)) {
            auto prevMove = board.applyMove(dir);
            auto prevFSM = fsm.applyMove(i);

            dfs(board, fsm, g+1, limit, nodeCount);

            fsm.undoMove(prevFSM);
            board.undoMove(prevMove);
        }
    }
}

double evaluateBranchFactorInner(StateMachineSimple &fsm, int width, int height) {
    auto startBoard = getAllStartingBoards(width, height)[0];

    std::vector<int> numNodes(1, 0);
    fsm.undoMove(0);
    auto ratio = 0.00;
    for (auto depth = 1; depth <= 22; ++depth) {
        int nodeCount = 0;
        dfs(startBoard, fsm, 0, depth, nodeCount);
        numNodes.resize(depth+1);
        numNodes[depth] = nodeCount;
        if (depth > 1) ratio = (double)numNodes[depth] / numNodes[depth-1];
        //DEBUG(depth << ", " << nodeCount << " (" << ratio << ")");
    }
    return ratio;
}

using TwoDStringVec = std::vector<std::vector<std::string>>;
void choose1Inner(const TwoDStringVec &choices, TwoDStringVec &result, std::vector<std::string> &current, std::size_t ind) {
    if (ind == choices.size()) {
        result.push_back(current);
        return;
    }
    for (auto choice: choices[ind]) {
        current.push_back(choice);
        choose1Inner(choices, result, current, ind+1);
        current.pop_back();
    }
}

TwoDStringVec choose1(const TwoDStringVec &choices) {
    TwoDStringVec result = {};
    std::vector<std::string> current = {};
    choose1Inner(choices, result, current, 0);
    return result;
}

#include <sstream>
std::string printVec(const std::vector<std::string> &strings) {
    std::stringstream ss;
    ss << "[";
    for (auto s: strings) ss << s << ", ";
    ss << "]";
    return ss.str();
}

void evaluateBranchFactor(StateMachineSimple &fsm, int width, int height) {
    /*
    CRINGE! 16 VS 16, (uldrul [-1, 0, -1, 0], ) VS (lurdlu [-1, 0, -1, 0], )
    CRINGE! 16 VS 16, (urdlur [-1, 0, 0, 1], ) VS (ruldru [-1, 0, 0, 1], )
    CRINGE! 16 VS 16, (ldruld [0, 1, -1, 0], ) VS (dlurdl [0, 1, -1, 0], )
    CRINGE! 16 VS 16, (rdlurd [0, 1, 0, 1], ) VS (druldr [0, 1, 0, 1], )
    */
    std::vector<std::string> strings = {"lr", "rl", "ud", "du"};
    TwoDStringVec choices = {
        {"uldrul", "lurdlu"},
        {"urdlur", "ruldru"},
        {"ldruld", "dlurdl"},
        {"rdlurd", "druldr"}
    };
    auto possibleFSMStrings = choose1(choices);

    for (auto possibility: possibleFSMStrings) {
        auto myStrings = strings;
        for (auto p: possibility) myStrings.push_back(p);
        auto myFSM = BuildFSMFromStrings(strings);
        auto theFSM = dfsOrderFSM(myFSM);
        //DEBUG(printVec(myStrings));
        //DEBUG("choice: " << evaluateBranchFactorInner(theFSM, width, height));
    }


    return;
}
