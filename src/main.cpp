#include <array>
#include <chrono>
#include <fstream>
#include <iostream>
#include <thread>
#include <unordered_map>
#include <vector>

#include "../include/Board.h"
#include "../include/DisjointDatabase.h"
#include "../include/Idastar.h"
#include "../include/InputParser.h"
#include "../include/Util.h"
#include "../include/WalkingDistance.h"

// Dynamic board size
// Dynamic database pattern

using namespace std;

void usage() {
    cout << "puzzle - An optimal 15 Puzzle solver\n"
            "\n"
            "Syntax:\n"
            "    puzzle [OPTIONS]\n"
            "\n"
            "Options:\n"
            "    -b <file>\n"
            "        Board files\n"
            "    -d <file>\n"
            "        Use database file\n"
            "    -h, --help\n"
            "        Print this help\n"
            "    -i, --interactive\n"
            "        Show a playback of each solution\n"
         << endl;
}

vector<vector<int>> readDatabase(istream& input) {
    int width, height, databaseNum;
    input >> width >> height >> databaseNum;

    vector<vector<int>> grids(databaseNum, vector<int>(width * height, 0));
    for (auto& grid : grids) {
        for (auto& tile : grid) {
            input >> tile;
        }
    }

    return grids;
}

vector<Board> readBoards(istream& input) {
    int width, height, boardNum;
    input >> width >> height >> boardNum;

    vector<Board> boards;
    for (int i = 0; i < boardNum; i++) {
        std::vector<int> board(width * height, 0);
        for (auto& tile : board) {
            input >> tile;
        }
        boards.emplace_back(board, width, height);
    }
    return boards;
}

std::pair<std::string, vector<vector<int>>> getDatabase() {
    if (InputParser::databaseExists()) {
        auto dbPath = InputParser::getDatabase();
        auto dbName = dbPath.substr(dbPath.find_last_of('/') + 1);

        ifstream input(dbPath);
        return {dbName, readDatabase(input)};
    }

    return {"def", readDatabase(cin)};
}

vector<Board> getBoards() {
    if (InputParser::boardExists()) {
        ifstream input(InputParser::getBoard());

        return readBoards(input);
    }

    return readBoards(cin);
}

int main(int argc, const char* argv[]) {
    InputParser::parse(argc, argv);

    // Help output
    if (InputParser::showHelp()) {
        usage();
        return 0;
    }

    // Reading database file
    const auto [dbName, grids] = getDatabase();
    if (grids.empty()) {
        cerr << "Error: must have at least one database" << endl;
        return 1;
    }

    // Setup database
    START_TIMER(db);
    DisjointDatabase::load(grids, 4, 4);
    END_TIMER(db);

    // Setup WD
    START_TIMER(wd);
    WalkingDistance::load(
        {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0}, 4, 4);
    END_TIMER(db);

    // Reading board file
    auto startBoards(getBoards());

    // Setup search
    Idastar search;

    // Start search
    vector<vector<Direction>> answers;
    START_TIMER(solve);
    for (const auto& startBoard : startBoards) {
        START_TIMER(singleSolve);
        auto solution = search.solve(startBoard);
        END_TIMER(singleSolve);

        if (solution.empty()) {
            cout << "No solution found!" << endl;
        } else {
            cout << "Solution: " << solution.size() << " steps" << endl;
            for (auto dir : solution) {
                cout << dir << " ";
            }
            cout << endl;
        }

        answers.push_back(solution);
    }
    END_TIMER(solve);

    // Check solutions
    cout << "Checking solutions:" << endl;
    for (auto i = 0; i < startBoards.size(); i++) {
        auto b = startBoards[i];
        auto solution = answers[i];

        for (auto j = solution.size(); j--;) {
            b.applyMove(solution[j]);
            if (InputParser::showInteractive()) {
                cout << (solution.size() - j) << endl;
                cout << b << endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
            }
        }
        cout << b << endl;
    }

    return 0;
}
