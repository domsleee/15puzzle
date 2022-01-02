#include "../include/WalkingDistance.h"

#include <algorithm>
#include <fstream>
#include <limits>
#include <map>

#include "../include/Util.h"

using Board = std::vector<int>;
using Table = std::vector<std::vector<int>>;
using Hash = std::string;
using Cost = WalkingDistance::Cost;
using Index = WalkingDistance::Index;
using RowColType = WalkingDistance::RowColType;

using WalkingDistance::col;
using WalkingDistance::costs;
using WalkingDistance::edgesDown;
using WalkingDistance::edgesUp;
using WalkingDistance::height;
using WalkingDistance::row;
using WalkingDistance::width;

std::map<Hash, int> tableIndexLookup;
std::vector<Hash> tables;
std::vector<Cost> WalkingDistance::costs;
std::vector<Index> WalkingDistance::edgesUp;
std::vector<Index> WalkingDistance::edgesDown;

std::vector<RowColType> WalkingDistance::row;  // Row #
std::vector<RowColType> WalkingDistance::col;  // Column #

int WalkingDistance::width;
int WalkingDistance::height;

Table calculateTable(const Board& grid, bool alongRow = true) {
    Table table(height, std::vector<int>(width, 0));

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int tile = grid[y * width + x];
            if (tile > 0) {
                if (alongRow) {
                    table[y][row[tile]]++;
                } else {
                    table[x][col[tile]]++;
                }
            }
        }
    }

    return table;
}

Hash calculateHash(const Table& table) {
    // Compress WD tables
    Hash hash = "";

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            hash += (char)table[y][x];
        }
    }

    return hash;
}

std::pair<Table, int> hashToTable(const Hash& hash) {
    Table table(height, std::vector<int>(width, 0));
    int rowSpace = 0;

    for (int y = 0; y < height; y++) {
        int count = 0;
        for (int x = 0; x < width; x++) {
            table[y][x] = (unsigned char)hash[y * width + x];
            count += table[y][x];
        }

        if (count == width - 1) {
            rowSpace = y;
        }
    }

    return {table, rowSpace};
}

void addHashToTables(const Hash& hash) {
    tableIndexLookup[hash] = tables.size();
    tables.push_back(hash);
}

int add(const Table& table, int cost) {
    auto hash = calculateHash(table);

    int tablesSize = (int)tables.size();
    int index = tablesSize;
    if (tableIndexLookup.find(hash) != tableIndexLookup.end()) index = tableIndexLookup[hash];

    // auto it = std::find(tables.cbegin(), tables.cend(), hash);
    // auto myIndex = std::distance(tables.cbegin(), it);

    // if (myIndex != index) {
    //     assertm(0, "epic fail");
    // }

    if (index == tablesSize) {
        addHashToTables(hash);
        costs.push_back(cost);
        for (auto i = 0; i < width; ++i) {
            edgesUp.push_back(std::numeric_limits<Index>::max());
            edgesDown.push_back(std::numeric_limits<Index>::max());
        }
    }

    return index;
}

void generate(const Board& goal) {
    // Start of BFS
    tables.clear();
    tableIndexLookup.clear();
    costs.clear();
    edgesUp.clear();
    edgesDown.clear();

    int lastPercentage = 0;

    // Initial table (goal)
    addHashToTables(calculateHash(calculateTable(goal)));
    costs.push_back(0);
    for (auto i = 0; i < width; ++i) {
        edgesUp.push_back(std::numeric_limits<Index>::max());
        edgesDown.push_back(std::numeric_limits<Index>::max());
    }

    for (std::size_t left = 0; left < tables.size(); left++) {
        int newPercentage = (100*left/tables.size());
        if (newPercentage != lastPercentage) {
            DEBUG("GENERATING LEFT " << left << " / " << tables.size() << " " << newPercentage << "%");
            lastPercentage = newPercentage;
        }
        auto cost = costs[left] + 1;
        auto [table, rowSpace] = hashToTable(tables[left]);

        if (int rowTile = rowSpace + 1; rowTile < height) {
            for (int x = 0; x < width; x++) {
                if (table[rowTile][x]) {
                    table[rowTile][x]--;
                    table[rowSpace][x]++;

                    auto index = add(table, cost);

                    edgesUp[left*width + x] = index;
                    edgesDown[index*width + x] = left;

                    table[rowTile][x]++;
                    table[rowSpace][x]--;
                }
            }
        }

        if (int rowTile = rowSpace - 1; rowTile >= 0) {
            for (int x = 0; x < width; x++) {
                if (table[rowTile][x]) {
                    table[rowTile][x]--;
                    table[rowSpace][x]++;

                    auto index = add(table, cost);

                    edgesDown[left*width + x] = index;
                    edgesUp[index*width + x] = left;

                    table[rowTile][x]++;
                    table[rowSpace][x]--;
                }
            }
        }
        if (tables.size() > std::numeric_limits<Index>::max()) {
            assertm(0, "table is too large for WalkingDistance::Index");
        }
    }

    tableIndexLookup.clear(); // only used for generate
}

void save(const std::string& filename) {
    std::ofstream file(filename, std::ios::out | std::ios::binary);
    if (!file.good()) {
        std::cerr << "Could not generate database file: databases/def-wd.dat"
                  << std::endl;
        return;
    }

    int size = tables.size();
    assertm((int)costs.size() == size, "Mismatching table sizes");
    assertm((int)edgesUp.size() == size, "Mismatching table sizes");
    assertm((int)edgesDown.size() == size, "Mismatching table sizes");
    file.write(reinterpret_cast<char*>(&size), sizeof(size));

    for (auto& table : tables) {
        auto size = table.size();
        file.write(reinterpret_cast<char*>(&size), sizeof(size));
        file.write(table.c_str(), size);
    }
    for (auto cost : costs) {
        file.write(reinterpret_cast<char*>(&cost), sizeof(cost));
    }
    for (auto i = 0; i < size * width; ++i) {
        file.write(reinterpret_cast<char*>(&edgesUp[i]), sizeof(Index));
    }
    for (auto i = 0; i < size * width; ++i) {
        file.write(reinterpret_cast<char*>(&edgesDown[i]), sizeof(Index));
    }
}

void WalkingDistance::load(const std::vector<int>& goal, int w, int h) {
    assertm(w == h, "Walking Distance requires square boards");

    width = w;
    height = h;
    auto length = w * h;

    assertm(width <= 255 && height <= 255, "row/col arrays are uint8_t");

    row.resize(length);
    col.resize(length);

    // Calculate row / column indices
    for (int i = 0; i < length; i++) {
        row[goal[i]] = i / width;
        col[goal[i]] = i % width;
    }

    std::string filename = "databases/" + std::to_string(w) + "-wd.dat";
    std::ifstream file(filename, std::ios::in | std::ios::binary);
    if (!file.good()) {
        // Database file missing, generate database
        DEBUG("Generating WD database");
        generate(goal);
        save(filename);
        DEBUG("Done generating WD");
        return;
    }

    // Read database from file
    DEBUG("Parsing database");

    // Store size in .dat file
    int size = 0;
    file.read(reinterpret_cast<char*>(&size), sizeof(size));

    tables.resize(size);
    costs.resize(size);
    edgesUp.resize(width * size);
    edgesDown.resize(width * size);

    for (auto& table : tables) {
        std::size_t tableSize;
        file.read(reinterpret_cast<char*>(&tableSize), sizeof(tableSize));
        table.resize(tableSize);
        file.read(table.data(), tableSize);
    }
    DEBUG("#tables: " << tables.size() << ", " << tables.capacity()
        << ", each table size: " << tables[0].size() << ", " << tables[0].capacity());

    for (auto& cost : costs) {
        file.read(reinterpret_cast<char*>(&cost), sizeof(cost));
    }
    for (auto i = 0; i < width * size; ++i) {
        file.read(reinterpret_cast<char*>(&edgesUp[i]), sizeof(Index));
    }
    for (auto i = 0; i < width * size; ++i) {
        file.read(reinterpret_cast<char*>(&edgesDown[i]), sizeof(Index));
    }
}

int WalkingDistance::getIndex(const Board& grid, bool alongRow) {
    auto hash = calculateHash(calculateTable(grid, alongRow));

    // Convert to index
    auto it = std::find(tables.cbegin(), tables.cend(), hash);
    assertm(it != tables.end(), "Missing walking distance table");
    auto index = std::distance(tables.cbegin(), it);

    return index;
}
