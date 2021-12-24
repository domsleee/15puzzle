#ifndef UTIL_H
#define UTIL_H

#include <array>
#include <cassert>
#include <chrono>
#include <iostream>
#include <vector>
#include <unordered_set>
#include <string>

#include "Direction.h"
#include "BoardRaw.h"
#include "Memory.h"

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

struct StringVectorCompare {
    inline bool operator()(const std::string& first,
            const std::string& second) const {
        return first.size() < second.size()
            || (first.size() == second.size() && first < second);
    }
};

#endif  // UTIL_H