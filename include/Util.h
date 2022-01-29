#ifndef UTIL_H
#define UTIL_H

#include <array>
#include <cassert>
#include <chrono>
#include <iostream>
#include <vector>
#include <unordered_set>
#include <string>
#include <unistd.h>

#include "Direction.h"
#include "BoardRaw.h"
#include "Memory.h"

#define FAIL(reason) { assertm(0, reason); exit(1); }
#define MAX_LL std::numeric_limits<long long>::max()

#define assertm(expr, msg) assert(((void)(msg), (expr)))
#define DEBUG(x) std::cout << x << '\n'
#define START_TIMER(name) auto timer_##name = std::chrono::steady_clock::now()
#define END_TIMER(name)                                                      \
    {                                                                        \
        std::cout << "Time elapsed: "                                        \
                  << (std::chrono::duration_cast<std::chrono::microseconds>( \
                          std::chrono::steady_clock::now() - timer_##name)   \
                          .count()) /                                        \
                         1000000.0                                           \
                  << "\n\n";                                                 \
    }

std::vector<int> combine(const std::vector<std::vector<int>>& grids);
int getBlank(const std::vector<int>& board);
std::vector<std::array<bool, 4>> calcMoveList(int width, int height);
Direction inverse(Direction move);
char directionToChar(Direction move);
Direction charToDirection(char move);
int charToInt(char move);
std::vector<BoardRaw> getAllStartingBoards(int width, int height);
void writeWordsToFile(std::string filename, const std::unordered_set<std::string> &words);
bool readWordsFromFile(std::string filename, std::unordered_set<std::string> &words);
bool readWordsFromFile(std::string filename, std::vector<std::string> &words);

struct StringVectorCompare {
    inline bool operator()(const std::string& first,
            const std::string& second) const {
        return first.size() < second.size()
            || (first.size() == second.size() && first < second);
    }
};

Direction pathMoved(const BoardRaw& a, const BoardRaw& b);



constexpr int getNumBitsPerTileConst(int boardSize) {
    auto numBits = 1;
    while (1 << (numBits) < boardSize) {
        numBits++;
    }
    return numBits;
}

int getNumBitsPerTile(int boardSize);
int getBitmask(int bitsPerTile);

#endif  // UTIL_H