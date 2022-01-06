#include <unordered_set>
#include <map>
#include <vector>
#include <string>
#include <set>

#include "AhoCorasick.h"
#include "BoardRep.h"
#include "ForbiddenWordsUtil.h"

struct ForbiddenWordsIDFS {
    long long depthLimit;
    int width, height;
    long long pathCount;
    std::vector<std::string> forbiddenWords;
    std::map<BoardRep, std::vector<CompressedPath>> boardToPaths;

    ForbiddenWordsIDFS(long long depthLimit, int width, int height);
    std::unordered_set<std::string> getForbiddenWords();
    void dfs(BoardRaw &board, std::string &path, int limit, StateMachine &fsm);
    void processAndClearBoardToPaths();
};
