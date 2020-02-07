#ifndef DISJOINTDATABASE_H
#define DISJOINTDATABASE_H

#include "Board.h"
#include "PartialDatabase.h"

#include <vector>

class DisjointDatabase {
private:
    std::vector<PartialDatabase*> databases;

public:
    DisjointDatabase(int len, std::string name,
                     std::vector<std::vector<std::vector<int>>> grids);
    virtual ~DisjointDatabase();

    int getHeuristic(const Board& board);
};

#endif  // DISJOINTDATABASE_H
