
#include <vector>
#include <map>
#include <cmath>

#include "Util.h"
#include "ForbiddenWordsUtil.h"

std::vector<double> boardProb44 = {
  0.02083, 0.04982, 0.04982, 0.02083,
  0.04982, 0.12953, 0.12953, 0.04982,
  0.04982, 0.12953, 0.12953, 0.04982,
  0.02083, 0.04982, 0.04982, 0.02083
};

std::vector<double> boardProb55 = {
  0.00708, 0.02185, 0.02717, 0.02185, 0.00708,
  0.02185, 0.06826, 0.07877, 0.06826, 0.02185,
  0.02717, 0.07877, 0.10012, 0.07877, 0.02717,
  0.02185, 0.06826, 0.07877, 0.06826, 0.02185,
  0.00708, 0.02185, 0.02717, 0.02185, 0.00708
};

double getScore(const std::string &forbiddenWord, int width) {
    auto probs = boardProb44;
    if (width == 4) {} // do nothing
    else if (width == 5) {
        probs = boardProb55;
    } else {
        DEBUG("not implemented for " << width);
        exit(0);
    }

    auto length = width * width;
    auto range = getCriticalPoints(forbiddenWord);
    double score = 0;
    for (auto i = 0; i < length; ++i) {
        auto r = i/width;
        auto c = i%width;
        auto mr = range.mr + r, Mr = range.Mr + r;
        auto mc = range.mc + c, Mc = range.Mc + c;
        if (mr < 0 || Mr >= width || mc < 0 || Mc >= width) continue;

        score += probs[i];
    }

    double sizeVal = pow(2.13, 50-forbiddenWord.size());
    return sizeVal + score;
}
