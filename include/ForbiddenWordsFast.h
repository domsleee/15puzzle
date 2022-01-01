#ifndef FORBIDDEN_WORDS_FAST_H
#define FORBIDDEN_WORDS_FAST_H

#include <unordered_set>
#include <vector>
#include "../include/Util.h"

#include "../include/BoardRaw.h"
#include "../include/ForbiddenWordsUtil.h"

class ForbiddenWordsFast {
    long long depthLimit;
    int width, height;
    std::unordered_set<std::string> getForbiddenWords(BoardRaw startBoard) const;

    std::vector<ValidationRet> validateDuplicateStrings(
        BoardRaw startBoard,
        const std::unordered_set<std::string> &strings
    );
    std::string getLLStr(long long val) const;
    std::string getDepthLimitStr() const {
        return getLLStr(depthLimit);
    }


public:
    ForbiddenWordsFast(long long depthLimit, int width, int height);
    std::unordered_set<std::string> getForbiddenWords();
    void printMessage() {
        DEBUG("Getting forbidden words with depthLimit " << getDepthLimitStr());
    }
    void validateDuplicateStrings(std::unordered_set<std::string> &strings);

};

#endif