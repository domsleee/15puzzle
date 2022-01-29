
#include <vector>
#include <map>
#include <cmath>
#include <algorithm>
    #include <sstream>


#include "Util.h"
#include "ForbiddenWordsUtil.h"
#include "StateMachineSimple.h"
#include "TileProbability.h"

static std::vector<double> boardProb44 = {
  0.02083, 0.04982, 0.04982, 0.02083,
  0.04982, 0.12953, 0.12953, 0.04982,
  0.04982, 0.12953, 0.12953, 0.04982,
  0.02083, 0.04982, 0.04982, 0.02083
};

static std::vector<double> boardProb55 = {
  0.02038, 0.02766, 0.04694, 0.02766, 0.02038,
  0.02766, 0.03967, 0.06368, 0.03967, 0.02766,
  0.04694, 0.06368, 0.09608, 0.06368, 0.04694,
  0.02766, 0.03967, 0.06368, 0.03967, 0.02766,
  0.02038, 0.02766, 0.04694, 0.02766, 0.02038
};

// two partitions
// FORBIDDEN
// PERMITTED

// for every FORBIDDEN operator, there must be at least one PERMITTED operator which has a
// length less than or equal to it, which bounding box is a subrange of the forbidden operator.

struct ForbiddenWordsScorer {
    std::vector<double> probs = {};
    int width;
    int height;
    int lastFSMCount;
    bool useEqualVal = false;

    ForbiddenWordsScorer(int width, int height)
        : width(width),
          height(height),
          lastFSMCount(0) {}

    void updateProbabilities(StateMachine &fsm) {
        auto fsmCount = fsm.outAndG.size();
        if (fsmCount != lastFSMCount) {
            DEBUG("need to update from " << lastFSMCount << " -> " << fsmCount);
            lastFSMCount = fsmCount;
            probs = getTileProbability(fsm, width, height);
        }
    }

    double getScore(const TwoPartition &twoPartition) {
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

    std::string prStringVec(const StringVec &vec) {
        std::stringstream ss;
        ss << "(";
        for (auto p: vec) {
            ss << p.decompress() << " " << getCriticalPoints(p.decompress()) << ", ";
        }
        ss << ")";
        return ss.str();
    }

    double getScore(const std::string &forbiddenWord, int width) {
        auto length = width * width;
        auto range = getCriticalPoints(forbiddenWord);
        double score = 0;
        for (auto i = 0; i < length; ++i) {
            auto r = i/width;
            auto c = i%width;
            auto mr = range.mr + r, Mr = range.Mr + r;
            auto mc = range.mc + c, Mc = range.Mc + c;
            if (mr < 0 || Mr >= width || mc < 0 || Mc >= width) continue;

            score += useEqualVal ? 1 : probs[i];
        }

        //double sizeVal = pow(2.13, 50-forbiddenWord.size());
        return score;
    }

    void scoreDetector(const std::vector<TwoPartition> &partitions) {
        useEqualVal = true;
        std::vector<std::pair<double, TwoPartition>> partitionPairs;
        for (const auto &partition: partitions) {
            partitionPairs.push_back({getScore(partition), partition});
        }
        std::sort(partitionPairs.begin(), partitionPairs.end(), std::greater<std::pair<double, TwoPartition>>());
        useEqualVal = false;

        if (partitionPairs.size() < 2) return;
        if (partitionPairs[0].first == 0) return;

        bool scoresEqual = partitionPairs[0].first == partitionPairs[1].first;
        if (!scoresEqual) return;

        auto actualScore0 = getScore(partitionPairs[0].second);
        auto actualScore1 = getScore(partitionPairs[1].second);
        if (actualScore0 == actualScore1) {
            DEBUG("CRINGE! " << partitionPairs[0].first << " VS " << partitionPairs[1].first << ", " << prStringVec(partitionPairs[0].second.first) << " VS " << prStringVec(partitionPairs[1].second.first));
        } else {
            DEBUG("DIFF!");
        }

    }

    TwoPartition getBestPartition(const std::vector<TwoPartition> &partitions) {
        scoreDetector(partitions);
        std::vector<std::pair<double, TwoPartition>> partitionPairs;
        for (const auto &partition: partitions) {
            partitionPairs.push_back({getScore(partition), partition});
        }
        std::sort(partitionPairs.begin(), partitionPairs.end(), std::greater<std::pair<double, TwoPartition>>());
        return partitionPairs[0].second;
    }
};
