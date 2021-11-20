#include "../include/ForbiddenWords.h"
#include "../include/BFSDefs.h"
#include "../include/Util.h"
#include "../include/Trie.h"

#include <queue>
#include <unordered_map>
#include <algorithm>

#define FORB_DEBUG(x)

ForbiddenWords::ForbiddenWords(int maxDepth, int width, int height)
    : maxDepth(maxDepth),
      width(width),
      height(height)
    {}

std::unordered_set<std::string> ForbiddenWords::getForbiddenWords() {
    std::unordered_set<std::string> uni, inter;
    auto startingBoards = getAllStartingBoards(width, height);

    for (const auto &startBoard: startingBoards) {
        auto r = getForbiddenWords(startBoard, maxDepth);
        for (auto s: r) uni.insert(s);
        FORB_DEBUG("Adding " << r.size() << " strings, total = " << uni.size());
        //break;
    }
    FORB_DEBUG("set union " << uni.size());
    removeDuplicateSuffixes(uni);
    validateDuplicateStrings(uni);
    FORB_DEBUG("total strings " << uni.size());

    return uni;
}

std::unordered_set<std::string> ForbiddenWords::getForbiddenWords(BoardRaw startBoard, int maxDepth) {
    std::unordered_set<std::string> res;
    std::unordered_set<BoardRaw> seen;
    auto q = std::queue<BFSNode>();
    int seenDepth = -1;
    seen.insert(startBoard);
    q.push(BFSNode{startBoard, 0, "", -1});
    while (!q.empty()) {
        auto front = q.front(); q.pop();
        if (front.dist > seenDepth) {
            seenDepth = front.dist;
            //DEBUG("depth: " << front.dist << " " << res.size());
            if (res.size() == 4) {
                //for (auto r : res) DEBUG(r);
            }
        }
        for (int i = 0; i < 4; i++) {
            auto dir = static_cast<Direction>(i);
            if (front.board.canMove(dir)) {
                auto newBoard = front.board;
                newBoard.applyMove(dir);
                auto newString = front.path + directionToChar(dir);
                if (seen.count(newBoard)) {
                    res.insert(newString);
                    continue;
                }
                auto newDist = front.dist + 1;
                if (newDist >= maxDepth) {
                    continue;
                }
                seen.insert(newBoard);
                q.push(BFSNode{newBoard, newDist, newString, static_cast<int>(inverse(dir))});
            }
        }
    }

    return res;
}

int ForbiddenWords::removeDuplicateSuffixes(std::unordered_set<std::string> &strings) {
    struct compare {
        inline bool operator()(const std::string& first,
                const std::string& second) const {
            return first.size() < second.size();
        }
    };
    
    std::unordered_set<std::string> stringsToRemove;
    std::vector<std::string> vec{strings.begin(), strings.end()};
    std::sort(vec.begin(), vec.end(), compare());
    Trie t;

    for (auto s: vec) {
        if (t.hasAnySuffix(s)) {
            stringsToRemove.insert(s);
            continue;
        }
        t.insertReverseIntoTrie(s);
    }
    DEBUG("Removed " << stringsToRemove.size() << " strings by suffix searching");
    for (auto string: stringsToRemove) {
        strings.erase(string);
    }

    return stringsToRemove.size();
}

bool ForbiddenWords::validateDuplicateStrings(std::unordered_set<std::string> &strings) {
    auto res = true;

    auto totalStringsRemoved = 0;
    for (const auto &startBoard: getAllStartingBoards(width, height)) {
        auto stringsRemoved = validateDuplicateStrings(startBoard, strings);
        FORB_DEBUG("strings removed by validation " << stringsRemoved);
        totalStringsRemoved += stringsRemoved;
    }
    DEBUG("Removed " << totalStringsRemoved << " strings by validation");
    
    return res;
}

int ForbiddenWords::validateDuplicateStrings(BoardRaw startBoard, std::unordered_set<std::string> &strings) {
    std::unordered_map<BoardRaw, int> shortestPathFromStrings;
    std::unordered_map<BoardRaw, std::vector<std::string>> boardToStrings;
    auto maxDepth = 0;

    for (auto s: strings) {
        maxDepth = std::max(maxDepth, static_cast<int>(s.size()));
        auto board = getBoardFromString(startBoard, s);
        if (board == nullptr) continue;
    
        int shortestPath = s.size();
        if (shortestPathFromStrings.count(*board) == 0 || shortestPath < shortestPathFromStrings[*board]) {
            shortestPathFromStrings[*board] = shortestPath;
        }
        if (boardToStrings.count(*board) == 0) {
            boardToStrings[*board] = {};
        }
        boardToStrings[*board].push_back(s);
    }

    auto q = std::queue<BFSNode>();
    std::unordered_map<BoardRaw, int> shortestPathFromBfs;

    shortestPathFromBfs[startBoard] = 0;
    q.push(BFSNode{startBoard, 0, "", -1});
    auto seenDepth = -1;

    while (!q.empty()) {
        auto front = q.front(); q.pop();
        if (front.dist > seenDepth) {
            //DEBUG("depth: " << front.dist << " " << shortestPathFromBfs.size());
            seenDepth = front.dist;
        }
         for (int i = 0; i < 4; i++) {
            auto dir = static_cast<Direction>(i);
            if (front.board.canMove(dir)) {
                auto newBoard = front.board;
                newBoard.applyMove(dir);
                auto newString = front.path + directionToChar(dir);
                if (shortestPathFromBfs.count(newBoard) || strings.count(newString)) {
                    continue;
                }
                auto newDist = front.dist + 1;
                shortestPathFromBfs[newBoard] = newDist;
                if (newDist >= maxDepth) {
                    continue;
                }

                q.push(BFSNode{newBoard, newDist, newString, static_cast<int>(inverse(dir))});
            }
        }
    }

    std::unordered_set<std::string> stringsToRemove; 
    for (auto [board, dist]: shortestPathFromStrings) {
        if (shortestPathFromBfs.count(board) == 0 || shortestPathFromBfs[board] > dist) {
            for (auto s: boardToStrings[board]) {
                stringsToRemove.insert(s);
            }
            continue;
        }
    }

    for (auto s: stringsToRemove) strings.erase(s);
    //for (auto s: stringsToRemove) DEBUG(s);
    return stringsToRemove.size();
}
