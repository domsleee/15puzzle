#include "../include/ForbiddenWordsIDFS.h"
#include "../include/BoardRaw.h"
#include "../include/BoardRep.h"
#include "../include/BoardRepTempl.h"
#include "../include/Direction.h"

#include "../include/AhoCorasick.h"
#include "../include/ForbiddenWordsUtil.h"
#include "../include/InputParser.h"

#include <algorithm>

BoardRaw getExploreBoard(int width);

template <int WIDTH>
ForbiddenWordsIDFS<WIDTH>::ForbiddenWordsIDFS(long long depthLimit, int width, int height)
    : depthLimit(depthLimit),
      width(width),
      height(height),
      pathCount(0),
      dfsCount(0),
      clearCount(0),
      isCleaning(false),
      startBoard(getExploreBoard(width))
{
}


template <int WIDTH>
std::unordered_set<std::string> ForbiddenWordsIDFS<WIDTH>::getForbiddenWords() {
    auto strFile = "databases/fsm-idfs-" + std::to_string(width) + "x" + std::to_string(height) + "-" + std::to_string(depthLimit);

    auto fsmFile = getFSMWordsFromFile(strFile);
    if (fsmFile.first == true) return fsmFile.second;

    auto startBoard = getExploreBoard(width);
    DEBUG("exploring " << startBoard);

    forbiddenWords = {};
    processAndClearBoardToPaths();
    for (auto limit = 1; limit <= depthLimit; ++limit) {
        clearCount = 0;
        DEBUG("paths length limit: " << limit << ", size: " << forbiddenWords.size());
        auto fsm = BuildFSMFromStrings(forbiddenWords);
        
        fsm.undoMove(0);
        std::string path;
        dfs(startBoard, path, limit, fsm);
        if (clearCount > 0) {
            clearMemory(limit, fsm);
        }
        processAndClearBoardToPaths();
    }

    DEBUG("FORBIDDEN WORDS SIZE " << forbiddenWords.size());
    writeWordsToFile(strFile, forbiddenWords);
    return forbiddenWords;
}

template <int WIDTH>
void ForbiddenWordsIDFS<WIDTH>::dfs(BoardRaw &board, std::string &path, int limit, StateMachine &fsm) {
    dfsCount++;
    auto range = getCriticalPoints(path); 
    if (range.Mr - range.mr >= width || range.Mc - range.mc >= width) {
        return;
    }
    
    auto boardRep = UseSubBoard::value 
        ? BoardRepT(board, getCoord(board))
        : BoardRepT(board);

    if (isCleaning) {
        if (boardToPaths.count(boardRep)) {
            auto &vec = boardToPaths.at(boardRep);
            auto compPath = CompressedPath(path);
            if (std::find(vec.begin(), vec.end(), compPath) == vec.end()) {
                vec.emplace_back(compPath);
                pathCount++;
            }
        }
    } else {
        auto &vec = boardToPaths.try_emplace(boardRep, StringVec()).first->second;
        auto compPath = CompressedPath(path);
        vec.emplace_back(compPath);
        pathCount++;

        if (shouldCleanUp()) {
            DEBUG("needs to clear early");
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

template <int WIDTH>
void ForbiddenWordsIDFS<WIDTH>::processAndClearBoardToPaths() {
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

template <int WIDTH>
void ForbiddenWordsIDFS<WIDTH>::clearMemory(int limit, StateMachine &fsm) {
    clearCount++;
    isCleaning = true;
    DEBUG("clearing memory..." << boardToPaths.size() << " boards with limit " << limit);

    auto oldFsmState = fsm.state;
    fsm.undoMove(0);
    auto startBoard = getExploreBoard(width);
    std::string path = "";
    dfsCount = 0;
    dfs(startBoard, path, limit, fsm);
    DEBUG("dfsCount: " << dfsCount);
    isCleaning = false;
    fsm.undoMove(oldFsmState);
    processAndClearBoardToPaths();
}

template <int WIDTH>
bool ForbiddenWordsIDFS<WIDTH>::shouldCleanUp() {
    if (isCleaning) return false;

    // forbiddenWords
    const int BTREE_MAP_OVERHEAD = 2;
    const int PTR_SIZE = sizeof(void*);
    const int MAP_OVERHEAD = 4;
    const int VECTOR_OVERHEAD = sizeof(std::size_t) + PTR_SIZE;
    long long forbiddenWordsEstimate = forbiddenWords.size() * (MAP_OVERHEAD + depthLimit + VECTOR_OVERHEAD);

    // boardToPaths
    long long pathMemoryEstimate = (boardToPaths.size() * VECTOR_OVERHEAD) + (CompressedPath::getByteEstimate(depthLimit) * pathCount);
    long long boardRepMemoryEstimate = boardToPaths.size() * (BTREE_MAP_OVERHEAD + BoardRepT::getByteEstimate());

    long long memoryEstimation = forbiddenWordsEstimate
        + pathMemoryEstimate
        + boardRepMemoryEstimate; // in bytes
    const long long tenGB = (long long)8 * 1000 * 1000 * 1000;
    if (memoryEstimation > tenGB) {
        return true;
    }
    return false;
}

template <int WIDTH>
BoardRaw ForbiddenWordsIDFS<WIDTH>::getExploreBoard(int width) {
    auto exploreWidth = width*2-1;
    auto length = exploreWidth * exploreWidth;
    std::vector<int> boardVec(length); // 4x4 ==> 7x7, 5x5 ==> 9x9

    if (UseSubBoard::value) {
        for (int r = 0; r < width; ++r) {
            for (int c = 0; c < width; ++c) {
                auto ind = r * exploreWidth + c;
                boardVec[ind] = r * width + c + 1;
            }
        }
        auto middle = (length) / 2;
        boardVec[middle] = 0;

        for (int r = 0; r < exploreWidth; ++r) {
            for (int c = 0; c < exploreWidth; ++c) {
                if (r < width && c < width) continue;

                auto ind = r * exploreWidth + c;
                auto fetchInd = -1;
                if (c >= width) {
                    fetchInd = ind - width;
                } else if (r >= width) {
                    fetchInd = ind - (width * exploreWidth);
                } else assertm(0, "??");

                boardVec[ind] = boardVec[fetchInd];
            }
        }
    } else {
        auto middle = (length) / 2;
        for (auto i = 0; i < length; ++i) {
            if (i == middle) boardVec[i] = 0;
            else boardVec[i] = i + 1 - (i > middle);
        }
    }

    //DEBUG(BoardRaw(boardVec, exploreWidth, exploreWidth)); exit(1);
    return BoardRaw(boardVec, exploreWidth, exploreWidth);
}

template <int WIDTH>
uint8_t ForbiddenWordsIDFS<WIDTH>::getCoord(const BoardRaw &board) const {
    auto exploreWidth = WIDTH*2-1;
    int mr = WIDTH-1, mc = WIDTH-1;
    for (int r = 0; r < exploreWidth; ++r) {
        for (int c = 0; c < exploreWidth; ++c) {
            auto ind = r * exploreWidth + c;
            auto boardVal = board.getBlankTile() == ind ? 0 : board.grid[ind];
            auto startVal = startBoard.getBlankTile() == ind ? 0 : startBoard.grid[ind];

            if (boardVal != startVal) {
                mc = std::min(mc, c);
                mr = std::min(mr, r);
            }
        }
    }
    return mr * exploreWidth + mc;
}


template class ForbiddenWordsIDFS<3>;
template class ForbiddenWordsIDFS<4>;
template class ForbiddenWordsIDFS<5>;
