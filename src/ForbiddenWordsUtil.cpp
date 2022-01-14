
#include "../include/ForbiddenWordsUtil.h"
#include "../include/ForbiddenWordsScore.h"
#include "../include/InputParser.h"

#include <fstream>
#include <filesystem>

ValidationRet::ValidationRet(int minBfsLength, int blankLocation, const std::vector<std::string> &stringsLessThanLength)
    : minBfsLength(minBfsLength),
      blankLocation(blankLocation),
      stringsLessThanLength(stringsLessThanLength)
    {}

// is b a subrange of a
bool isSubRange(const Range &a, const Range &b) {
    return b.mr >= a.mr
        && b.Mr <= a.Mr
        && b.mc >= a.mc
        && b.Mc <= a.Mc;
}

Range getCriticalPoints(const std::string &s) {
    auto r = 0, c = 0;
    auto mr = 0, Mr = 0, mc = 0, Mc = 0;
    for (auto ch: s) {
        switch(ch) {
            case 'l': c--; mc = std::min(mc, c); break;
            case 'r': c++; Mc = std::max(Mc, c); break;
            case 'u': r--; mr = std::min(mr, r); break;
            case 'd': r++; Mr = std::max(Mr, r); break;
            default: throw "what";
        }
    }
    return {mr, Mr, mc, Mc};
}

Range& Range::unionWith(const Range &otherRange) {
    mr = std::min(mr, otherRange.mr);
    Mr = std::max(Mr, otherRange.Mr);
    mc = std::min(mc, otherRange.mc);
    Mc = std::max(Mc, otherRange.Mc);
    return *this;
}

void build2Partitions(std::vector<TwoPartition> &res, TwoPartition &current, const StringVec &strings, std::size_t ind) {
    if (ind == strings.size()) {
        res.push_back(current);
        return;
    }

    current.first.push_back(strings[ind]);
    build2Partitions(res, current, strings, ind+1);
    current.first.pop_back();
    current.second.push_back(strings[ind]);
    build2Partitions(res, current, strings, ind+1);
    current.second.pop_back();
}

std::vector<TwoPartition> get2Partitions(const StringVec &strings) {
    std::vector<TwoPartition> res = {};
    TwoPartition current = {{}, {}};
    build2Partitions(res, current, strings, 0);
    return res;
}


// two partitions
// FORBIDDEN
// PERMITTED

// for every FORBIDDEN operator, there must be at least one PERMITTED operator which has a
// length less than or equal to it, which bounding box is a subrange of the forbidden operator.

double getScore(const TwoPartition &twoPartition, int width) {
    for (auto &forbiddenWordComp: twoPartition.first) {
        auto forbiddenWord = forbiddenWordComp.decompress();

        std::vector<std::string> filt;
        for (auto perm: twoPartition.second) {
            if (perm.decompress().size() <= forbiddenWord.size()) filt.push_back(perm.decompress());
        }

        if (filt.size() == 0) return INVALID_PARTITION;

        bool ok = false;
        auto forbiddenWordRange = getCriticalPoints(forbiddenWord);
        for (auto perm: filt) {
            auto permittedRange = getCriticalPoints(perm);
            if (isSubRange(forbiddenWordRange, permittedRange)) {
                ok = true;
                break;
            }
        }
        if (ok) continue;
        return INVALID_PARTITION;
    }

    double score = 0;
    for (auto &forbiddenWord: twoPartition.first) {
        score += getScore(forbiddenWord.decompress(), width);
    }
    return score;
}

std::pair<bool, std::vector<std::string>> getFSMWordsFromFile(const std::string &filename) {
    std::vector<std::string> forbiddenWords = {};
    if (InputParser::fsmFileExists()) {
        auto strFile = InputParser::getFSMFile();
        if (!readWordsFromFile(filename, forbiddenWords)) {
            DEBUG("could not read from file " << strFile);
            exit(1);
        };
        DEBUG("Loaded FSM from file " << filename << " #words: " << forbiddenWords.size());
        return {true, forbiddenWords};
    }

    if (readWordsFromFile(filename, forbiddenWords)) {
        DEBUG("Loaded FSM from file " << filename << " #words: " << forbiddenWords.size());
        return {true, forbiddenWords};
    }
    return {false, {}};
}

void writePathsToFile(std::string filename, const std::set<CompressedPath> &paths) {
    if (std::filesystem::exists(filename)) {
        DEBUG("refused to write words to file that already existed " << filename);
        return;
    }
    std::ofstream fout{filename};
    for (auto s: paths) fout << s.decompress() << '\n';
}
