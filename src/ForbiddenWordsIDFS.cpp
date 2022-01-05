#include "../include/ForbiddenWordsIDFS.h"
#include "../include/BoardRaw.h"
#include "../include/BoardRep.h"
#include "../include/Direction.h"

#include "../include/AhoCorasick.h"
#include "../include/ForbiddenWordsUtil.h"

#include <algorithm>

BoardRaw getExploreBoard(int width);

ForbiddenWordsIDFS::ForbiddenWordsIDFS(long long depthLimit, int width, int height)
    : depthLimit(depthLimit),
      width(width),
      height(height)
{
}


std::unordered_set<std::string> ForbiddenWordsIDFS::getForbiddenWords() {
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
            //DEBUG("OK");
            /*if (paths.size() >= 2) {
                //DEBUG("PATHS " << paths.size() << ": " << boardRep.toBoard());
                if (limit == 2) {
                    for (auto i = 1; i < paths.size(); ++i) forbiddenWords.push_back(paths[i]);
                } else {
                    //for (auto p: paths) DEBUG(p << ": " << toStr(getCriticalPoints(p)));
                    if (paths.size() == 2) {
                        auto bounding0 = getCriticalPoints(paths[0]);
                        auto bounding1 = getCriticalPoints(paths[1]);
                        if (paths[0].size() != paths[1].size()) {
                            // we must take the longer path
                            auto mPath = paths[0].size() < paths[1].size() ? paths[0] : paths[1];
                            auto MPath = paths[0].size() < paths[1].size() ? paths[1] : paths[0];

                            if (isSubRange(getCriticalPoints(mPath), getCriticalPoints(MPath))) {
                                forbiddenWords.push_back(MPath);
                            } else {
                                DEBUG("not subrange.." << mPath << " " << getCriticalPoints(mPath) << ", " << MPath << " " << getCriticalPoints(MPath));
                                //exit(1);
                            }

                        } else {
                            auto sub01 = isSubRange(bounding0, bounding1);
                            auto sub10 = isSubRange(bounding1, bounding0);

                            if (sub01 && sub10) {
                                forbiddenWords.push_back(paths[1]);
                                both++;
                            }
                            else if (sub01) {
                                forbiddenWords.push_back(paths[1]);
                            } else if (sub10) {
                                forbiddenWords.push_back(paths[0]);
                            } else {
                                DEBUG("NEITHER ARE SUBRANGE!");
                               // exit(1);
                            }
                        }

                        
                    } else {
                        DEBUG("NOT IMPLEMENTED: " << paths.size());
                        throwout += paths.size();
                        //throw "not implemented";
                    }
                }
            }*/
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

    return {forbiddenWords.begin(), forbiddenWords.end()};
}

// two partitions
// FORBIDDEN
// PERMITTED

// for every FORBIDDEN operator, there must be at least one PERMITTED operator which has a
// length less than or equal to it. The range of the forbidden operator must be a subrange to the
// range formed by the union of the permitted operator strings.



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