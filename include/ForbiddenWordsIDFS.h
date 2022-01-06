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
    long long pathCount, dfsCt;
    std::unordered_set<std::string> forbiddenWords;
    std::set<BoardRep> boardsWeCareAbout;
    std::map<BoardRep, std::vector<CompressedPath>> boardToPaths;

    ForbiddenWordsIDFS(long long depthLimit, int width, int height);
    std::unordered_set<std::string> getForbiddenWords();
    void dfs(BoardRaw &board, std::string &path, int limit, StateMachine &fsm);
    void processAndClearBoardToPaths();
    void clearMemory(int limit, StateMachine &fsm);
    bool shouldCleanUp();
};
