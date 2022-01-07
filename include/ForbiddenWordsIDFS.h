#include <unordered_set>
#include <map>
#include <vector>
#include <string>
#include <set>

#include "AhoCorasick.h"
#include "BoardRep.h"
#include "BoardRepTempl.h"
#include "ForbiddenWordsUtil.h"
#include "../third_party/btree/map.h"


template <int WIDTH=5>
struct ForbiddenWordsIDFS {
    using BoardRepT = BoardRepTempl<(WIDTH*2-1)>;
    long long depthLimit;
    int width, height;
    long long pathCount, dfsCt;
    std::unordered_set<std::string> forbiddenWords;
    std::set<BoardRepT> boardsWeCareAbout;
    std::map<BoardRepT, std::vector<CompressedPath>> boardToPaths;

    ForbiddenWordsIDFS(long long depthLimit, int width, int height);
    std::unordered_set<std::string> getForbiddenWords();
    void dfs(BoardRaw &board, std::string &path, int limit, StateMachine &fsm);
    void processAndClearBoardToPaths();
    void clearMemory(int limit, StateMachine &fsm);
    bool shouldCleanUp();
};
