#ifndef FORBIDDEN_WORDS_UTIL_H
#define FORBIDDEN_WORDS_UTIL_H

#include <unordered_set>
#include <vector>
#include <string>
#include <iostream>
#include <set>
#include "Util.h"

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

struct CompressedPath {
    uint8_t size;
    std::vector<uint8_t> vec;
    
    CompressedPath(const std::string &path) {
        size = path.size();
        auto vecSize = ((path.size() * 2) + 7) / 8;
        vec.assign(vecSize, 0);

        for (std::size_t i = 0; i < path.size(); ++i) {
            auto ind = i/4;
            auto subInd = i%4;
            vec[ind] |= myCharToInt(path[i]) << (6 - subInd*2);
        }
    }

    std::string decompress() const {
        std::string res(size, 'a');
        for (std::size_t i = 0; i < size; ++i) {
            auto subInd = i%4;
            auto val = (vec[i/4] >> (6 - subInd*2)) & 0b11;
            res[i] = myIntToChar(val);
        }
        return res;
    }

    int myCharToInt(char c) const {
        switch(c) {
            case 'd': return 0;
            case 'l': return 1;
            case 'r': return 2;
            case 'u': return 3;
        }
        FAIL("unknown char");
    }

    int myIntToChar(int i) const {
        switch(i) {
            case 0: return 'd';
            case 1: return 'l';
            case 2: return 'r';
            case 3: return 'u';
        }
        FAIL("unknown int");
    }

    friend bool operator==(const CompressedPath &lhs, const CompressedPath &rhs) {
        if (lhs.size != rhs.size) return false;
        for (std::size_t i = 0; i < lhs.vec.size(); ++i) {
            if (lhs.vec[i] != rhs.vec[i]) return false;
        }
        return true;
    }

    bool operator<(const CompressedPath &other) const {
        if (size != other.size) return size < other.size;
        for (std::size_t i = 0; i < vec.size(); ++i) {
            if (vec[i] != other.vec[i]) return vec[i] < other.vec[i];
        }
        return false;
    }

    static int getByteEstimate(int maxLength) {
        return sizeof(void*) + ((maxLength * 2) + 7) / 8;
    }
};

bool isSubRange(const Range &a, const Range &b);
Range getCriticalPoints(const std::string &s);

using StringVec = std::vector<CompressedPath>;
using TwoPartition = std::pair<StringVec, StringVec>;
std::vector<TwoPartition> get2Partitions(const StringVec &strings); 

const int INVALID_PARTITION = -1;

std::pair<bool, std::vector<std::string>> getFSMWordsFromFile(const std::string &filename);

void writePathsToFile(std::string filename, const std::set<CompressedPath> &paths);

#endif