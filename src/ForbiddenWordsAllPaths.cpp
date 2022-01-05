#include "../include/ForbiddenWordsAllPaths.h"
#include "../include/BoardRep.h"
#include "../include/Direction.h"
#include "../include/Trie.h"

#include <unordered_map>
#include <unordered_map>

#include <queue>
#include <stack>
#include <algorithm>

#define FORB3_DEBUG(x)


using Path = std::vector<Direction>;

namespace std {
    template <>
    struct hash<Path> {
        size_t operator()(const Path& v) const {
            // see https://stackoverflow.com/questions/20511347/a-good-hash-function-for-a-vector
            std::size_t seed = v.size();
            for(auto& i : v) {
                seed ^= static_cast<int>(i) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
            return seed;
        }
    };
}


void buildPaths(std::vector<Path> &out, Path &currPath, const PredType &pred, const BoardRep &board);
std::vector<Path> getAllPaths(const BoardRep &board, const PredType &pred);


ForbiddenWordsAllPaths::ForbiddenWordsAllPaths(long long depthLimit, int width, int height)
    : depthLimit(depthLimit),
      width(width),
      height(height)
{
}

std::unordered_set<std::string> ForbiddenWordsAllPaths::getForbiddenWords() {
    forbiddenWords.clear();
    auto startBoards = getAllStartingBoards(width, height);
    return getForbiddenWords(startBoards[0]);
}

std::unordered_set<std::string> ForbiddenWordsAllPaths::getForbiddenWords(BoardRaw startBoard) {
    PredType pred;
    std::unordered_map<BoardRep, int> dist;
    std::vector<BoardRep> boardsToDealWith = {};

    std::queue<BoardRep> q;
    auto startBoardRep = BoardRep{startBoard};
    q.push(startBoardRep);
    dist.emplace(startBoardRep, 0);
    auto knownDist = 0;

    while (!q.empty()) {
        auto top = q.front(); q.pop();
        auto myDist = dist[top];

        if (myDist > knownDist) {
            DEBUG("dealing with boards " << knownDist);
            dealWithBoards(boardsToDealWith, pred);
            knownDist = myDist;
        }
    
        auto board = top.toBoard();
        for (auto i = 0; i < 4; ++i) {
            auto dir = static_cast<Direction>(i);
            if (!board.canMove(dir)) continue;

            auto newBoard = board;
            newBoard.applyMove(dir);
            auto newBoardRep = BoardRep{newBoard};
            auto newDist = myDist + 1;
            if (newDist >= depthLimit) {
                break;
            }
            if (dist.count(newBoardRep)) {
                if (dist.at(newBoardRep) < newDist) continue;
                pred.at(newBoardRep).push_back(top);
                boardsToDealWith.push_back(newBoardRep);
            } else {
                dist.emplace(newBoardRep, newDist);
                std::vector<BoardRep> newPaths = {top};
                pred.emplace(newBoardRep, newPaths);
            }
            q.push(newBoardRep);
        }
    }

    exit(1);
    return {};
}

std::string pathToString(const Path &path) {
    std::string str = "";
    for (int i = path.size()-1; i >= 0; --i) {
        str.push_back(directionToChar(path[i]));
    }
    return str;
}

void ForbiddenWordsAllPaths::dealWithBoards(std::vector<BoardRep> &boardsToDealWith, PredType &pred) {
    std::sort(boardsToDealWith.begin(), boardsToDealWith.end());
    std::unique(boardsToDealWith.begin(), boardsToDealWith.end());

    Trie t;
    for (auto s: forbiddenWords) {
        t.insertReverseIntoTrie(s);
    }

    for (const auto &board: boardsToDealWith) {
        auto paths = getAllPaths(board, pred);
        std::unordered_set<Path> pathSet = {paths.begin(), paths.end()};
        std::unordered_set<Path> pathsToRemove = {};
        for (const auto &path: pathSet) {
            auto str = pathToString(path);
            if (t.hasAnySuffix(str)) {
                pathsToRemove.insert(path);
            }
        }
        for (const auto &path: pathsToRemove) pathSet.erase(path);

        if (paths.size() > 1) {
            DEBUG("paths size " << paths.size());
        }
        forbiddenWords.push_back(pathToString(paths[0]));
        DEBUG("forbidden words size " << forbiddenWords.size());
    }
    boardsToDealWith.clear();
}



std::vector<Path> getAllPaths(const BoardRep &board, const PredType &pred) {
    std::vector<Path> paths;
    Path currPath = {};
    buildPaths(paths, currPath, pred, board);
    return paths;
}

void buildPaths(std::vector<Path> &out, Path &currPath, const PredType &pred, const BoardRep &board) {
    if (pred.count(board) == 0) {
        out.push_back(currPath);
        return;
    }

    for (auto p: pred.at(board)) {
        auto dir = pathMoved(board.toBoard(), p.toBoard());
        currPath.push_back(dir);
        buildPaths(out, currPath, pred, p);
    }
}

