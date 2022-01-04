#include <unordered_set>
#include <unordered_map>
#include <vector>
#include "BoardRaw.h"
#include "BoardRep.h"

using PredType = std::unordered_map<BoardRep, std::vector<BoardRep>>;

struct ForbiddenWordsAllPaths {
    long long depthLimit;
    int width, height;
    std::vector<std::string> forbiddenWords;

    ForbiddenWordsAllPaths(long long depthLimit, int width, int height);
    std::unordered_set<std::string> getForbiddenWords();
    std::unordered_set<std::string> getForbiddenWords(BoardRaw startBoard);
    void dealWithBoards(std::vector<BoardRep> &boardsToDealWith, PredType &pred);
};
