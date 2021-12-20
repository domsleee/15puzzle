#ifndef FORBIDDEN_WORDS_UTIL_H
#define FORBIDDEN_WORDS_UTIL_H

#include <unordered_set>
#include <vector>
#include <string>

struct ValidationRet {
    int minBfsLength, blankLocation;
    std::vector<std::string> stringsLessThanLength;

    ValidationRet(int minBfsLength, int blankLocation, const std::vector<std::string> &stringsLessThanLength);
};

struct StringVectorCompare {
    inline bool operator()(const std::string& first,
            const std::string& second) const {
        return first.size() < second.size()
            || (first.size() == second.size() && first < second);
    }
};

#endif