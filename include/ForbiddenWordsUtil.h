#ifndef FORBIDDEN_WORDS_UTIL_H
#define FORBIDDEN_WORDS_UTIL_H

#include <unordered_set>
#include <vector>
#include <string>
#include <iostream>

struct ValidationRet {
    int minBfsLength, blankLocation;
    std::vector<std::string> stringsLessThanLength;

    ValidationRet(int minBfsLength, int blankLocation, const std::vector<std::string> &stringsLessThanLength);
};

struct Range {
    int mr;
    int Mr;
    int mc;
    int Mc;
    Range(int mr, int Mr, int mc, int Mc)
        : mr(mr),
          Mr(Mr),
          mc(mc),
          Mc(Mc)
          {}
    
    friend std::ostream& operator<<(std::ostream& out, const Range& range) {
        out << "["
            << range.mr << ", "
            << range.Mr << ", "
            << range.mc << ", "
            << range.Mc << "]";
        return out; 
    }

    Range& unionWith(const Range &otherRange);
};

bool isSubRange(const Range &a, const Range &b);
Range getCriticalPoints(const std::string &s);

using StringVec = std::vector<std::string>;
using TwoPartition = std::pair<StringVec, StringVec>;
std::vector<TwoPartition> get2Partitions(const StringVec &strings); 

const int INVALID_PARTITION = -1;
double getScore(const TwoPartition &twoPartition, int width);

#endif