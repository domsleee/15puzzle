#include "../include/FSMPruner.h"
#include "../include/ForbiddenWords.h"
#include "../include/Util.h"
#include "../include/BFSDefs.h"

#include <queue>
#include <vector> 

#include <unordered_map>

void validateIdeas();
void validateFSM(StateMachine &fsm, std::unordered_set<std::string> &words);

StateMachine FSMPruner::go() {
    auto maxDepth = 2;
    DEBUG("Getting forbidden words with depth: " << maxDepth);
    START_TIMER(forbiddenWords);
    auto forbiddenWords = ForbiddenWords(maxDepth);
    auto strings = forbiddenWords.getForbiddenWords();
    DEBUG("found " << strings.size() << " forbidden strings");
    END_TIMER(forbiddenWords);

    DEBUG("Building FSM from strings");
    START_TIMER(FSM);
    auto fsm = BuildFSMFromStrings(strings);
    END_TIMER(FSM);

    //validateFSM(fsm, strings);

    //stateMachine.applyMove((int)charToDirection('r'));
    //DEBUG("should be false " << stateMachine.canMove((int)charToDirection('l')));
    return fsm;
}

void validateIdeas() {
    std::unordered_set<std::string> words = {
        "llrd",
        "lr"
    };
    auto sm = BuildFSMFromStrings(words);
    sm.applyMove(charToInt('l'));
    sm.applyMove(charToInt('l'));
    DEBUG("can move: " << sm.canMove(charToInt('r')));
    assertm(!sm.canMove(charToInt('r')), "should not be able to move right here.");
}

void validateFSM(StateMachine &fsm, std::unordered_set<std::string> &words) {
    for (auto w1: words) {
        for (auto w2: words) {
            if (w1 == w2) continue;
            auto lastC = w2[w2.size()-1];
            w2 = w2.substr(0, w2.size()-1);
            for (auto i = 1; i < w1.size(); ++i) {
                auto subw1 = w1.substr(0, i);
                fsm.undoMove(0);
                auto broken = false;
                for (auto c: subw1) {
                    if (!fsm.canMove(charToInt(c))) {
                        broken = 1;
                        break;
                    }
                    fsm.applyMove(charToInt(c));
                }
                if (broken) continue;
                for (auto c: w2) {
                    if (!fsm.canMove(charToInt(c))) {
                        broken = 1;
                        break;
                    }
                    fsm.applyMove(charToInt(c));
                }
                if (broken) continue;
                DEBUG("checking " << subw1 << " " << w2 << lastC);
                assertm(!fsm.canMove(charToInt(lastC)), "should be marked as word.");
            }
        }
    }
}
