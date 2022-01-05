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
}


std::unordered_set<std::string> ForbiddenWordsIDFS::getForbiddenWords() {
    auto strFile = "databases/fsm-idfs-" + std::to_string(width) + "x" + std::to_string(height) + "-" + std::to_string(depthLimit);
    std::unordered_set<std::string> setOfWords;

    if (InputParser::fsmFileExists()) {
        auto strFile = InputParser::getFSMFile();
        if (!readWordsFromFile(strFile, setOfWords)) {
            DEBUG("could not read from file " << strFile);
            exit(1);
        };
        DEBUG("Loaded FSM from file " << strFile << " #words: " << setOfWords.size());
        return setOfWords;
    }

    if (readWordsFromFile(strFile, setOfWords)) {
        DEBUG("Loaded FSM from file " << strFile << " #words: " << setOfWords.size());
        return setOfWords;
    }

    auto startBoard = getExploreBoard(width);
    DEBUG("exploring " << startBoard);


    forbiddenWords = {};
    boardToPaths = {};
    int both = 0, throwout = 0;
    for (auto limit = 1; limit <= depthLimit; ++limit) {
        DEBUG("paths length limit: " << limit << ", size: " << forbiddenWords.size());
        auto fsm = BuildFSMFromStrings({forbiddenWords.begin(), forbiddenWords.end()});
        boardToPaths.clear();
        fsm.undoMove(0);

        std::string path;
        dfs(startBoard, path, limit, fsm);

        for (const auto &[boardRep, paths]: boardToPaths) {
            if (paths.size() < 2) continue;

            auto partitions = get2Partitions(paths);
            std::vector<std::pair<double, TwoPartition>> partitionPairs;
            for (auto &partition: partitions) {
                partitionPairs.push_back({getScore(partition, width), partition});
            }
            std::sort(partitionPairs.begin(), partitionPairs.end(), std::greater<std::pair<double, TwoPartition>>());

            auto best = partitionPairs[0];
            if (best.first == INVALID_PARTITION) {
                DEBUG("no valid partition");
            } else {
                for (auto &s: best.second.first) forbiddenWords.push_back(s);
            }
        }
    }

    DEBUG("FORBIDDEN WORDS SIZE " << forbiddenWords.size());
    std::sort(forbiddenWords.begin(), forbiddenWords.end(), StringVectorCompare());
    //for (auto &s: forbiddenWords) std::cout << s << '\n';
    DEBUG("FORBIDDEN WORDS SIZE " << forbiddenWords.size() << ", throwout: " << throwout << ", both: " << both);

    setOfWords = {forbiddenWords.begin(), forbiddenWords.end()};
    writeWordsToFile(strFile, setOfWords);

    return setOfWords;
}

void ForbiddenWordsIDFS::dfs(BoardRaw &board, std::string &path, int limit, StateMachine &fsm) {
    auto range = getCriticalPoints(path); 
    if (range.Mr - range.mr >= width || range.Mc - range.mc >= width) {
        //DEBUG("THROWING OUT" );
        //DEBUG(path << " " << toStr(vec));
        //exit(1);
        return;
    }
    
    auto boardRep = BoardRep(board);
    if (!boardToPaths.count(boardRep)) boardToPaths[boardRep] = {};
    boardToPaths.at(boardRep).push_back(path);

    if (path.size() == limit) {
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