#include "../include/ForbiddenWordsFast.h"
#include "../include/ForbiddenWords.h"
#include "../include/BFSDefs.h"
#include "../include/BoardRep.h"

#include "../include/Util.h"
#include "../include/Trie.h"
#include "../include/AhoCorasick.h"
#include "../include/InputParser.h"


#include <queue>
#include <unordered_map>
#include <map>
#include <algorithm>
#include <execution>
#include <unordered_set>

#define FORB2_DEBUG(x)

void debugInsertString(
    const std::string &s,
    const BoardRep &startBoardRep,
    const BoardRep &boardRep,
    const Direction lastDir,
    const std::map<BoardRep, BoardRep> &pred);


ForbiddenWordsFast::ForbiddenWordsFast(long long depthLimit, int width, int height)
    : depthLimit(depthLimit),
      width(width),
      height(height)
    {
    }

std::string ForbiddenWordsFast::getLLStr(long long val) const {
    return val == MAX_LL ? "MAX" : std::to_string(val);
}


std::unordered_set<std::string> ForbiddenWordsFast::getForbiddenWords() {
    std::unordered_set<std::string> uni;
    if (InputParser::fsmFileExists()) {
        auto strFile = InputParser::getFSMFile();
        if (!readWordsFromFile(strFile, uni)) {
            DEBUG("could not read from file " << strFile);
            exit(1);
        };
        DEBUG("Loaded FSM from file " << strFile << " #words: " << uni.size());
        return uni;
    }

    auto depthLimitStr = getDepthLimitStr();
    auto strFile = "databases/fsm-" + std::to_string(width) + "x" + std::to_string(height) + "-" + depthLimitStr;
    auto beforeValidationFile = strFile + "-beforevalidation";

    if (readWordsFromFile(strFile, uni)) {
        DEBUG("Loaded FSM from file " << strFile << " #words: " << uni.size());
        return uni;
    } else if (readWordsFromFile(beforeValidationFile, uni))
    {
        // do nothing
        DEBUG("Loaded FSM before validation from file");
    }
    else {
        auto startingBoards = getAllStartingBoards(width, height);
        uni = std::accumulate(
            startingBoards.cbegin(),
            startingBoards.cend(),
            std::unordered_set<std::string>(),
            [this](std::unordered_set<std::string> a, const BoardRaw &startBoard) -> std::unordered_set<std::string> {
                auto words = getForbiddenWords(startBoard);
                for (const auto &s: words) a.insert(s);
                auto forb = ForbiddenWords(0, 0, 0); 
                forb.removeDuplicateSuffixes(a);
                return a;
            }
        );
    }
    
    FORB2_DEBUG("set union " << uni.size());
    auto forb = ForbiddenWords(0, 0, 0); 
    forb.removeDuplicateSuffixes(uni);
    writeWordsToFile(beforeValidationFile, uni);
    validateDuplicateStrings(uni);
    FORB2_DEBUG("total strings " << uni.size());
    writeWordsToFile(strFile, uni);
    return uni;
}

std::string getString(
    const BoardRep &startBoardRep,
    const BoardRep &boardRep,
    const std::map<BoardRep, BoardRep> &pred)
{
    assertm(pred.count(boardRep) > 0, "ok??");
    auto currBoardRep = boardRep;
    std::string s = "";

    while (currBoardRep != startBoardRep) {
        auto prevBoardRep = currBoardRep;
        currBoardRep = pred.at(currBoardRep);
        auto currBoard = currBoardRep.toBoard();
        auto prevBoard = prevBoardRep.toBoard();

        auto dir = Direction::D;

        if (currBoard.getBlankTile() == prevBoard.getBlankTile() + 1) dir = Direction::L;
        else if (currBoard.getBlankTile() == prevBoard.getBlankTile() - 1) dir = Direction::R;
        else if (currBoard.getBlankTile() == prevBoard.getBlankTile() - prevBoard.getWidth()) dir = Direction::D;
        else if (currBoard.getBlankTile() == prevBoard.getBlankTile() + prevBoard.getWidth()) dir = Direction::U;
        else {
            DEBUG("BLANKS " << currBoard.getBlankTile() << " VS " << prevBoard.getBlankTile() << ", width" << prevBoard.getWidth());
            assertm(0, "no direction found??");
        }

        //DEBUG("PREPEND " << directionToChar(dir) << " TO " << s);
        s = directionToChar(dir) + s;
    }

    return s;
}

void maybeInsertString(
    std::unordered_set<std::string> &res,
    Trie &trie,
    const BoardRep &startBoardRep,
    const BoardRep &boardRep,
    const Direction lastDir,
    const std::map<BoardRep, BoardRep> &pred)
{
    std::string s = getString(startBoardRep, boardRep, pred)
        + std::string(1, directionToChar(lastDir));
    
    if (res.count(s) || trie.hasAnySuffix(s)) return;

    debugInsertString(s, startBoardRep, boardRep, lastDir, pred);

    //FORB2_DEBUG("INSERT STRING " << s);
    trie.insertReverseIntoTrie(s);
    res.insert(s);
}

std::unordered_set<std::string> ForbiddenWordsFast::getForbiddenWords(BoardRaw startBoard) const {
    std::unordered_set<std::string> res;
    std::map<BoardRep, BoardRep> pred;
    auto q = std::queue<std::pair<int, BoardRep>>();
    int maxKnownDepth = -1;
    auto startBoardRep = BoardRep{startBoard};
    q.push({0, startBoardRep});
    pred.emplace(startBoardRep, startBoardRep);
    Trie trie;

    while (!q.empty()) {
        
        //if (it % 100 && getVirtualUsage() % 10000 < 100) DEBUG(getVirtualUsage() << " VS " << memLimit);
        auto [depth, front] = q.front(); q.pop();

        if (depth > maxKnownDepth) {
            maxKnownDepth = depth;
            DEBUG("new depth " << maxKnownDepth);
        }

        auto board = front.toBoard();
        for (int i = 0; i < 4; i++) {
            auto dir = static_cast<Direction>(i);
            if (board.canMove(dir)) {
                auto newBoard = board;
                newBoard.applyMove(dir);
                auto newBoardRep = BoardRep{newBoard};
                if (pred.count(newBoardRep)) {
                    maybeInsertString(res, trie, startBoardRep, board, dir, pred);
                    continue;
                }
                pred.emplace(newBoardRep, front);

                auto newDepth = depth + 1;
                if (newDepth >= depthLimit) {
                    continue;
                }                
                q.push({newDepth, newBoardRep});
            }
        }
    }

    return res;
}


void ForbiddenWordsFast::validateDuplicateStrings(std::unordered_set<std::string> &strings) {
    auto startingBoards = getAllStartingBoards(width, height);

    std::vector<std::vector<ValidationRet>> res(startingBoards.size());
    std::transform(
        std::execution::seq,
        startingBoards.cbegin(),
        startingBoards.cend(),
        res.begin(),
        [&](const BoardRaw &startBoard) -> std::vector<ValidationRet> {
            return validateDuplicateStrings(startBoard, strings);
        }
    );

    auto stringSize = strings.size();
    for (const auto &vec: res) {
        for (const auto &validationRet: vec) {
            auto stringToRemove = validationRet.stringsLessThanLength[validationRet.stringsLessThanLength.size()-1];
            if (strings.count(stringToRemove) == 0) continue;
            FORB2_DEBUG("remove " << validationRet.blankLocation << ", minLength "
                << validationRet.minBfsLength
                << " #strings: " << validationRet.stringsLessThanLength.size()
                << ", string: " << stringToRemove);
            strings.erase(stringToRemove);
        }
    }

    auto totalStringsRemoved = stringSize - strings.size();
    DEBUG("Removed " << totalStringsRemoved << " strings by validation");

    return;
}

long long skipCount = 0, skipLen = 0;
std::vector<ValidationRet> ForbiddenWordsFast::validateDuplicateStrings(BoardRaw startBoard, const std::unordered_set<std::string> &strings) {
    auto maxDepth = 0;
    for (auto s: strings) {
        maxDepth = std::max(maxDepth, static_cast<int>(s.size()));
    }

    DEBUG("BUILDING FSM for validation...");
    auto fsm = BuildFSMFromStrings(strings);
    DEBUG("FSM Built");

    auto q = std::queue<std::pair<int, BoardRep>>();
    std::unordered_map<BoardRep, int> shortestPathFromBfs;

    auto startBoardRep = BoardRep{startBoard};
    shortestPathFromBfs[startBoardRep] = 0;
    q.push({0, startBoardRep});

    DEBUG("search begin");
    auto maxKnownDepth = -1;
    while (!q.empty()) {
        auto [lastFSM, frontBoardRep] = q.front(); q.pop();
        auto board = frontBoardRep.toBoard();
         for (int i = 0; i < 4; i++) {
            fsm.undoMove(lastFSM);
            auto dir = static_cast<Direction>(i);
            if (board.canMove(dir) && fsm.canMove(i)) {
                auto newBoard = board;
                newBoard.applyMove(dir);
                auto newBoardRep = BoardRep{newBoard};
                if (shortestPathFromBfs.count(newBoardRep)) {
                    continue;
                }
                auto newDist = shortestPathFromBfs.at(frontBoardRep) + 1;
                shortestPathFromBfs.emplace(newBoardRep, newDist);
                if (newDist >= maxDepth) {
                    continue;
                }
                if (newDist > maxKnownDepth) {
                    maxKnownDepth = newDist;
                    DEBUG("new depth " << maxKnownDepth);
                }

                fsm.applyMove(i);
                q.push({fsm.state, newBoardRep});
            }
        }
    }

    DEBUG("SEARCH FINISHED");

    std::unordered_map<BoardRep, std::vector<std::string>> boardToStrings;
    std::unordered_map<BoardRep, int> shortestPathFromStrings;
    for (auto s: strings) {
        maxDepth = std::max(maxDepth, static_cast<int>(s.size()));
        auto board = getBoardFromString(startBoard, s);
        if (board == nullptr) continue;
        auto boardRep = BoardRep{*board}; // 6766239
    
        int shortestPath = s.size();
        if (shortestPathFromStrings.count(boardRep) == 0 || shortestPath < shortestPathFromStrings[boardRep]) {
            shortestPathFromStrings[boardRep] = shortestPath;
        }
        if (boardToStrings.count(boardRep) == 0) {
            boardToStrings[boardRep] = {};
        }
        boardToStrings[boardRep].push_back(s);
    }

    std::vector<ValidationRet> ret;
    for (auto &[board, dist]: shortestPathFromStrings) {
        int shortestPathLength = shortestPathFromBfs.count(board) ? shortestPathFromBfs.at(board) : -1;
        if (shortestPathLength == -1 || shortestPathLength > dist) {
            std::vector<std::string> strings = {};
            for (auto s: boardToStrings[board]) {
                int stringSize = s.size();
                if (shortestPathLength == -1 || stringSize <= shortestPathLength) {
                    skipCount++;
                    skipLen += s.size();
                    strings.push_back(s);
                }
            }
            if (strings.size() == 0) continue;
            std::sort(strings.begin(), strings.end(), StringVectorCompare());
            ret.push_back({shortestPathLength, startBoard.getBlankTile(), strings});
        }
    }
    DEBUG("Skipped count " << skipCount << ", skipLen: " << skipLen);

    return ret;
}

std::unordered_set<std::string> debugInsertStringWordList = {};

#include <fstream>

std::vector<int> getCriticalPoints(std::string s) {
    std::vector<int> res;
    auto r = 0, c = 0;
    auto mr = 0, Mr = 0, mc = 0, Mc = 0;
    for (auto ch: s) {
        auto dir = charToDirection(ch);
        switch(dir) {
            case Direction::L: c--; break;
            case Direction::R: c++; break;
            case Direction::U: r--; break;
            case Direction::D: r++; break;
            default: assertm(0, "unknonw.");
        }
        mr = std::min(mr, r);
        Mr = std::max(Mr, r);
        mc = std::min(mc, c);
        Mc = std::max(Mc, c);
    }
    return {mr, Mr, mc, Mc};
}

// is `b` a subrange of `a`
bool isSubRange(const std::vector<int> &a, const std::vector<int> &b) {
    return b[0] >= a[0]
        && b[1] <= a[1]
        && b[2] >= a[2]
        && b[3] <= a[3];
}


// a-b
std::unordered_set<int> setDiff(const std::unordered_set<int> &a, const std::unordered_set<int> &b) {
    std::unordered_set<int> res(a.begin(), a.end());
    for (auto s: b) res.erase(s);
    return res;
}


void debugInsertString(
    const std::string &s,
    const BoardRep &startBoardRep,
    const BoardRep &boardRep,
    const Direction lastDir,
    const std::map<BoardRep, BoardRep> &pred) {
    
    return;
    DEBUG("???");
    exit(1);
    
    if (debugInsertStringWordList.size() == 0) {
        std::ifstream fin("results/insertStringWordList.txt");
        std::string t;
        while (fin >> t) {
            debugInsertStringWordList.insert(t);
        }
    }

    if (debugInsertStringWordList.count(s)/*s == "dllurdrulldru"*/) { // matches: ullddrulurrdl
        // debugging
        auto nxBoard = boardRep.toBoard();
        nxBoard.applyMove(lastDir);
        auto nxBoardRep = BoardRep{nxBoard};
        //DEBUG("INSERTING STRING " << startBoardRep.toBoard().getBlankTile());
        auto otherString = getString(startBoardRep, nxBoardRep, pred);
        //DEBUG(startBoardRep.toBoard().getBlankTile() << ":" << s << " vs " << otherString);

        auto startBoard = startBoardRep.toBoard();
        auto a = getCriticalPoints(s), b = getCriticalPoints(otherString);

        if (false && (isSubRange(a, b) || isSubRange(b, a))) {
            // one is a subset of the other
        } else {
            //if (s.size() != otherString.size()) return;
            FORB2_DEBUG("NOT SUBRANGE!");
            FORB2_DEBUG(startBoardRep.toBoard().getBlankTile() << ":" << s << " vs " << otherString);
            //for (auto a1: a) std::cout << a1 << ' '; std::cout << '\n';
            //for (auto b1: b) std::cout << b1 << ' '; std::cout << '\n';
        }
    }
}
