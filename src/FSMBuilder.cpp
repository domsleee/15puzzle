#include "../include/FSMBuilder.h"
#include "../include/ForbiddenWords.h"
#include "../include/Util.h"

FSMBuilder::FSMBuilder(int width, int height, int maxDepth):
    width(width),
    height(height),
    maxDepth(maxDepth)
    {}

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

    return fsm;
}
