#include "../include/ForbiddenWordsIDFS.h"
#include "../include/BoardRaw.h"
#include "../include/BoardRep.h"
#include "../include/Direction.h"

#include "../include/AhoCorasick.h"
#include "../include/ForbiddenWordsUtil.h"
#include "../include/InputParser.h"

#include <algorithm>

BoardRaw getExploreBoard(int width);

ForbiddenWordsIDFS::ForbiddenWordsIDFS(long long depthLimit, int width, int height)
    : depthLimit(depthLimit),
      width(width),
      height(height)
{
    pathCount = 0;
    dfsCt = 0;
    boardsWeCareAbout = {};
}


std::unordered_set<std::string> ForbiddenWordsIDFS::getForbiddenWords() {
    auto strFile = "databases/fsm-idfs-" + std::to_string(width) + "x" + std::to_string(height) + "-" + std::to_string(depthLimit);

    auto fsmFile = getFSMWordsFromFile(strFile);
    if (fsmFile.first == true) return fsmFile.second;

    auto startBoard = getExploreBoard(width);
    DEBUG("exploring " << startBoard);

    forbiddenWords = {};
    processAndClearBoardToPaths();
    for (auto limit = 1; limit <= depthLimit; ++limit) {
        DEBUG("paths length limit: " << limit << ", size: " << forbiddenWords.size());
        auto fsm = BuildFSMFromStrings(forbiddenWords);
        
        fsm.undoMove(0);
        std::string path;
        dfs(startBoard, path, limit, fsm);
        clearMemory(limit, fsm);
        processAndClearBoardToPaths();
    }

    DEBUG("FORBIDDEN WORDS SIZE " << forbiddenWords.size());
    writeWordsToFile(strFile, forbiddenWords);
    return forbiddenWords;
}

void ForbiddenWordsIDFS::dfs(BoardRaw &board, std::string &path, int limit, StateMachine &fsm) {
    dfsCt++;
    auto range = getCriticalPoints(path); 
    if (range.Mr - range.mr >= width || range.Mc - range.mc >= width) {
        return;
    }
    
    auto boardRep = BoardRep(board);
    auto isCleaning = boardsWeCareAbout.size() > 0;

    if (isCleaning) {
        if (boardsWeCareAbout.count(boardRep)) {
            auto &vec = boardToPaths.try_emplace(boardRep, StringVec()).first->second;
            auto compPath = CompressedPath(path);
            if (std::find(vec.begin(), vec.end(), compPath) == vec.end()) {
                vec.emplace_back(compPath);
                pathCount += path.size();
            }
        }
    } else {
        auto &vec = boardToPaths.try_emplace(boardRep, StringVec()).first->second;
        auto compPath = CompressedPath(path);
        vec.emplace_back(compPath);
        pathCount += path.size();

        if (shouldCleanUp()) {
            clearMemory(limit, fsm);
        }
    }

    if (path.size() == static_cast<std::size_t>(limit)) {
        return;
    }

    for (auto i = 0; i < 4; ++i) {
        auto dir = static_cast<Direction>(i);
        if (!fsm.canMove(i) || !board.canMove(dir)) continue;

        auto oldState = fsm.applyMove(i);
        auto oldBoard = board.applyMove(dir);
        path.push_back(directionToChar(dir));

        dfs(board, path, limit, fsm);

        fsm.undoMove(oldState);
        board.undoMove(oldBoard);
        path.pop_back();
    }
}

void ForbiddenWordsIDFS::processAndClearBoardToPaths() {
    while (boardToPaths.size() > 0) {
        const auto [boardRep, paths] = *boardToPaths.begin();
        if (paths.size() >= 2) {
            auto partitions = get2Partitions(paths);
            std::vector<std::pair<double, TwoPartition>> partitionPairs;
            for (const auto &partition: partitions) {
                partitionPairs.push_back({getScore(partition, width), partition});
            }
            std::sort(partitionPairs.begin(), partitionPairs.end(), std::greater<std::pair<double, TwoPartition>>());

            auto best = partitionPairs[0];
            // no invalid partition... the forbiddenwords can be empty
            for (const auto &s: best.second.first) {
                forbiddenWords.emplace(s.decompress());
            }
        }

        boardToPaths.erase(boardRep);
    }
    DEBUG("after clearing... #words: " << forbiddenWords.size());
    pathCount = 0;
}

void ForbiddenWordsIDFS::clearMemory(int limit, StateMachine &fsm) {
    for (const auto &[boardRep, paths]: boardToPaths) {
        boardsWeCareAbout.insert(boardRep);
    }
    DEBUG("clearing memory..." << boardsWeCareAbout.size() << " boards with limit " << limit);

    auto oldFsmState = fsm.state;
    fsm.undoMove(0);
    auto startBoard = getExploreBoard(width);
    std::string path = "";
    dfsCt = 0;
    dfs(startBoard, path, limit, fsm);
    DEBUG("dfsCt: " << dfsCt);
    fsm.undoMove(oldFsmState);
    processAndClearBoardToPaths();
    boardsWeCareAbout.clear();
}

bool ForbiddenWordsIDFS::shouldCleanUp() {
    if (boardsWeCareAbout.size() > 0) return false;

    long long pathMemoryEstimate = (pathCount * 2 + 7) / 8 + (4 * pathCount/15);
    auto exploreWidth = width*2-1;
    auto length = exploreWidth * exploreWidth;
    long long boardRepMemoryEstimate = boardToPaths.size() * (4 + length / 2);

    long long memoryEstimation = pathMemoryEstimate + boardRepMemoryEstimate; // in bytes
    const long long tenGB = (long long)2 * 1000 * 1000 * 1000;
    if (memoryEstimation > tenGB) {
        return true;
    }
    return false;
}

BoardRaw getExploreBoard(int width) {
    auto exploreWidth = width*2-1;
    auto length = exploreWidth * exploreWidth;
    std::vector<int> boardVec(length); // 4x4 ==> 7x7, 5x5 ==> 9x9
    
    auto middle = (length) / 2;
    for (auto i = 0; i < length; ++i) {
        if (i == middle) boardVec[i] = 0;
        else boardVec[i] = i + 1 - (i > middle);
    }

    return BoardRaw(boardVec, exploreWidth, exploreWidth);
}