#ifndef FORBIDDEN_WORDS_FAST_H
#define FORBIDDEN_WORDS_FAST_H

#include <unordered_set>
#include <vector>
#include "../include/Util.h"

#include "../include/BoardRaw.h"
#include "../include/ForbiddenWordsUtil.h"

class ForbiddenWordsFast {
    long long memLimit, itLimit;
    int width, height;
    std::unordered_set<std::string> getForbiddenWords(BoardRaw startBoard);

    std::vector<ValidationRet> validateDuplicateStrings(
        BoardRaw startBoard,
        const std::unordered_set<std::string> &strings
    );
    std::string getLLStr(long long val) const;
    std::string getItLimitStr() const {
        return getLLStr(itLimit);
    }
    std::string getMemLimitStr() const {
        return memLimit == MAX_LL
            ? "MAX"
            : std::to_string(memLimit/(long long)1e6) + "mb";
    }


public:
    ForbiddenWordsFast(long long memLimit, long long itLimit, int width, int height);
    std::unordered_set<std::string> getForbiddenWords();
    void printMessage() {

        DEBUG("Getting forbidden words with memLimit " << getMemLimitStr() << " and itLimit " << getItLimitStr());
    }
    void validateDuplicateStrings(std::unordered_set<std::string> &strings);

};

#endif