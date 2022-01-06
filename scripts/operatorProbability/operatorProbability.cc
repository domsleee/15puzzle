
#include <math.h>       /* pow */
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>

const int NUM_ROWS = 4;
const int NUM_COLS = 4;

std::vector<char> boardMap = {
    'c', 's', 's', 'c',
    's', 'm', 'm', 's',
    's', 'm', 'm', 's',
    'c', 's', 's', 'c'
};

std::map<char, double> probs = {
    {'c', 0.0887/4.0},
    {'s', 0.4614/8.0},
    {'m', 0.4498/4.0}
};

std::vector<int> getCriticalPoints(std::string s) {
    std::vector<int> res;
    auto r = 0, c = 0;
    auto mr = 0, Mr = 0, mc = 0, Mc = 0;
    for (auto ch: s) {
        switch(ch) {
            case 'l': c--; mc = std::min(mc, c); break;
            case 'r': c++; Mc = std::max(Mc, c); break;
            case 'u': r--; mr = std::min(mr, r); break;
            case 'd': r++; Mr = std::max(Mr, r); break;
            default: throw "What";
        }
    }
    return {mr, Mr, mc, Mc};
}

int diffSigns(int a, int b) {
    return (a ^ b) < 0 ? 1 : 0;
}

double getNumSquares(const std::string &a) {
    double res = 0;
    auto crit = getCriticalPoints(a); // [0,2,0,2] is a 3x3 rectangle --> 4 critical points
    for (auto i = 0; i < 16; ++i) {
        auto r = i/NUM_COLS;
        auto c = i%NUM_COLS;
        auto mr = crit[0] + r, Mr = crit[1] + r;
        auto mc = crit[2] + c, Mc = crit[3] + c;
        if (mr < 0 || Mr >= NUM_ROWS || mc < 0 || Mc >= NUM_COLS) continue;

        res += probs[boardMap[i]];
    }

    return res;
    //auto width = WIDTH - (crit[1]-crit[0]);
    //auto height = HEIGHT - (crit[3]-crit[2]);
    //return width*height + diffSigns(crit[0], crit[1]) + diffSigns(crit[2], crit[3]);
}

double getValue(const std::string &a) {
    double sizeVal = pow(2.13, 30-a.size());
    return sizeVal + getNumSquares(a);
}

int main() {
    std::string s;
    std::vector<std::string> words;
    std::vector<std::pair<double, std::string>> wordVals;
    while (std::cin >> s) {
        words.push_back(s);
        wordVals.push_back({getValue(s), s});
    }
    
    std::stable_sort(wordVals.begin(), wordVals.end(), std::greater<std::pair<double, std::string>>());
    for (auto i = 0; i < 25000; ++i) std::cout << wordVals[i].second << '\n';

}