#include "../include/PartialDatabase.h"

#include "../include/Pattern.h"

#include <chrono>
#include <fstream>
#include <iostream>
#include <queue>
#include <unordered_map>

PartialDatabase::PartialDatabase(std::vector<std::vector<int>> grid,
                                 std::string dbName, int index)
    : filename("databases/" + dbName + "-" + std::to_string(index) + ".dat"),
      pattern(Pattern(grid)) {
    int count = 0;
    std::unordered_map<int, int> cells;
    for (int y = 0; y < pattern.HEIGHT; y++) {
        for (int x = 0; x < pattern.WIDTH; x++) {
            if (grid[y][x] > 0) {
                cells[grid[y][x]] = count++;
                tiles.push_back(grid[y][x]);
            }
        }
    }

    std::ifstream file(filename, std::ios::in | std::ios::binary);
    if (!file.good()) {
        // Database file missing, generate database
        std::cout << "Generating database" << std::endl;
        generateDists();
        saveDists();
    } else {
        // Read database from file
        std::cout << "Parsing database" << std::endl;

        uint64_t id = 0;
        int dist;
        while (file.read((char*)&dist, sizeof(dist))) {
            distMap[id] = dist;
            id++;
        }

        if (id != pattern.size()) {
            std::cout << "Error: incorrectly sized database!" << std::endl;
            throw;
        }
    }

    std::cout << "Number of entries: " << distMap.size() << std::endl;

    std::cout << "Cells:" << std::endl;
    for (auto i : cells) {
        std::cout << i.first << " " << i.second << std::endl;
    }

    std::cout << "Pattern:" << std::endl << pattern << std::endl;
}

void PartialDatabase::generateDists() {
    struct State {
        Pattern board;
        int dist;
    };

    // The pattern database consists of all possible permutations
    // of the pattern's tiles placed in the board. Any tile that
    // isn't part of the pattern are treated as blanks, so that
    // the board only contains the pattern's tiles.
    // To generate the database, it starts from the solved state
    // and runs BFS, sliding the pattern's tiles around
    // to generate all pattern states with heuristic 1,
    // then 2, etc. This ensures that if the search encounters
    // a new pattern state, it was reached with the minimum
    // number of slides.

    // board has 0 for irrelevant cells, non-0 for those part of the database
    int count = 0;
    int dist = 0;

    std::cout << "Generating database for: " << std::endl;
    std::cout << pattern << std::endl;

    auto startTime = std::chrono::steady_clock::now();

    // Start of algorithm
    distMap = std::vector<int>(pattern.size(), 1000000);
    std::queue<State> bfs;

    bfs.push({pattern, 0});
    distMap[pattern.id] = 0;

    while (!bfs.empty()) {
        State curr = bfs.front();
        bfs.pop();

        // Logging
        if (curr.dist > dist) {
            std::cout << dist << ": " << count << std::endl;
            dist = curr.dist;
            count = 1;
        } else {
            count++;
        }

        for (int i = 0; i < curr.board.tiles.size(); i++) {
            for (int j = 0; j < 4; j++) {
                Direction dir = static_cast<Direction>(j);
                if (curr.board.canShift(i, dir)) {
                    State next = {curr.board, curr.dist + 1};
                    next.board.shiftCell(i, dir);

                    // Haven't found this board yet
                    if (distMap[next.board.id] == 1000000) {
                        distMap[next.board.id] = next.dist;
                        bfs.push(next);
                    }
                }
            }
        }
    }

    auto endTime = std::chrono::steady_clock::now();
    auto duration = (std::chrono::duration_cast<std::chrono::microseconds>(
                         endTime - startTime)
                         .count()) /
                    1000000.0;
    std::cout << "Time taken: " << duration << std::endl;
}

void PartialDatabase::saveDists() {
    // Store file
    std::ofstream file(filename, std::ios::out | std::ios::binary);
    if (!file.good()) {
        std::cerr << "Could not generate database file: " + filename
                  << std::endl;
    } else {
        // Board dimensions (width, height)
        // file << board.grid[0].size() << " " << board.grid.size() << endl;
        // Number of cells, locations of cells (id)
        // file << board.cells.size() << " " << board.getId() << endl;
        for (auto dist : distMap) {
            if (dist == 1000000) {
                std::cout << "Error: missing entries!" << std::endl;
                throw;
            }
            file.write((char*)&dist, sizeof(dist));
        }

        file.close();
    }
}

PartialDatabase::~PartialDatabase() {}
