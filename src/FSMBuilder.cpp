#include "../include/FSMBuilder.h"
#include "../include/ForbiddenWords.h"
#include "../include/Util.h"

FSMBuilder::FSMBuilder(int width, int height):
    width(width),
    height(height)
    {}

StateMachine FSMBuilder::build() {
    auto maxDepth = 1;
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
