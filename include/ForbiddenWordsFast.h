#ifndef FORBIDDEN_WORDS_FAST_H
#define FORBIDDEN_WORDS_FAST_H

#include <unordered_set>
#include <vector>
#include "../include/Util.h"

#include "../include/BoardRaw.h"
#include "../include/ForbiddenWordsUtil.h"

class ForbiddenWordsFast {
    long long qsizeLimit;
    int width, height;
    std::unordered_set<std::string> getForbiddenWords(BoardRaw startBoard);

    std::vector<ValidationRet> validateDuplicateStrings(
        BoardRaw startBoard,
        const std::unordered_set<std::string> &strings
    );

public:
    ForbiddenWordsFast(long long qsizeLimit, int width, int height);
    std::unordered_set<std::string> getForbiddenWords();
    void printMessage() { DEBUG("Getting forbidden words with qsizeLimit " << (qsizeLimit/1e6) << "mb"); }
    void validateDuplicateStrings(std::unordered_set<std::string> &strings);

};

#endif