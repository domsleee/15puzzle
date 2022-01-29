#pragma once

#include <unordered_set>
#include <map>
#include <vector>
#include <string>
#include <set>
#include <type_traits>

#include "AhoCorasick.h"
#include "BoardRep.h"
#include "BoardRepTempl.h"
#include "BoardRepSub.h"
#include "ForbiddenWordsUtil.h"
#include "ForbiddenWordsScore.h"
#include "../third_party/btree/map.h"
#include "../third_party/btree/set.h"


template <int WIDTH=5>
struct ForbiddenWordsIDFS {
    //using BoardRepT = BoardRepTempl<(WIDTH*2-1)>;
    using BoardRepT = BoardRepSub<WIDTH>; // BoardRepTempl<(WIDTH*2-1)>;
    using UseSubBoard = std::is_same<BoardRepT, BoardRepSub<WIDTH>>;
    

    const long long depthLimit;
    const int width, height;
    long long pathCount, dfsCount, clearCount;
    bool isCleaning;
    std::set<CompressedPath> forbiddenWords;
    const BoardRaw startBoard;
    btree::map<BoardRepT, std::vector<CompressedPath>> boardToPaths;
    ForbiddenWordsScorer forbiddenWordsScorer;

    ForbiddenWordsIDFS(long long depthLimit, int width, int height);
    std::vector<std::string> getForbiddenWords();
    void dfs(BoardRaw &board, std::string &path, int limit, StateMachine &fsm);
    void processAndClearBoardToPaths();
    void clearMemory(int limit, StateMachine &fsm);
    bool shouldCleanUp();
    uint8_t getCoord(const BoardRaw &board) const;
    BoardRaw getExploreBoard(int width);
};

#include "../src/ForbiddenWordsIDFS.tpp"
