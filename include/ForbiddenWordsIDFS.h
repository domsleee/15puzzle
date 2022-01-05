#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <string>

#include "AhoCorasick.h"
#include "BoardRep.h"

struct ForbiddenWordsIDFS {
    long long depthLimit;
    int width, height;
    std::vector<std::string> forbiddenWords;
    std::unordered_map<BoardRep, std::vector<std::string>> boardToPaths;

    ForbiddenWordsIDFS(long long depthLimit, int width, int height);
    std::unordered_set<std::string> getForbiddenWords();
    void dfs(BoardRaw &board, std::string &path, int limit, StateMachine &fsm);
};
