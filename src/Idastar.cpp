#include "../include/Idastar.h"

#include <iostream>

#define INF 1000000

Idastar::Idastar() : path({}), minCost(INF), limit(0), nodes(0) {}

std::vector<Direction> Idastar::solve(Board start) {
    // Uses IDA* with additive pattern disjoint database heuristics
    std::cout << "Running single threaded" << std::endl;

    path.clear();
    nodes = 1;
    limit = start.getHeuristic();

    if (limit > 0) {
        std::cout << "Limit, Nodes:";

        // Starting moves (for prevMove)
        const std::vector<Direction> startMoves = start.getMoves();

        // Boards after first move
        std::vector<Board> firstSlide;
        for (auto startDir : startMoves) {
            auto copy = start;
            copy.applyMove(startDir);
            firstSlide.push_back(copy);
        }

        while (path.empty()) {
            minCost = INF;
            std::cout << " " << limit << ", " << nodes << std::endl;
            for (size_t i = 0; i < startMoves.size(); i++) {
                if (dfs(firstSlide[i], 1, startMoves[i])) {
                    path.push_back(startMoves[i]);
                    break;
                }
            }
            limit = minCost;
        }

        std::cout << "Nodes expanded: " << nodes << std::endl;
    } else {
        std::cout << "Already solved" << std::endl;
    }

    return path;
}

bool Idastar::dfs(Board& node, int g, Direction prevMove) {
    int h = node.getHeuristic();
    int f = g + h;

    if (f <= limit) {
        if (h == 0) {
            // Found goal state (heuristic = 0)
            return true;
        }
    } else {
        // Exceeded search depth, store next smallest depth
        if (f < minCost) {
            minCost = f;
        }
        return false;
    }
    nodes += 1;

    const auto& moves = node.getMoves(prevMove);
    for (auto move : moves) {
        int cost = node.applyMove(move);

        if (dfs(node, g + cost, move)) {
            path.push_back(move);
            return true;
        }

        node.undoMove(move);
    }

    return false;
}

Idastar::~Idastar() = default;
