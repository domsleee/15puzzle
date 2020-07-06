#ifndef PATTERN_H
#define PATTERN_H

#include <unordered_map>
#include <vector>

#include "Direction.h"

// Minimal state info
struct Pattern {
    std::vector<unsigned> pos;  // Value to position mapping
    uint64_t id;                // distMap key
    uint64_t g;                 // Position to value mapping
};

// Stores pre-computed values for family of Patterns
class PatternGroup {
    std::vector<int> deltas;
    const unsigned WIDTH;
    const unsigned HEIGHT;

public:
    PatternGroup(std::vector<std::vector<unsigned>> grid, const unsigned WIDTH,
                 const unsigned HEIGHT);

    Pattern initPattern;  // Initial pattern

    unsigned getCell(const Pattern& pattern, int position) const;
    void setCell(Pattern& pattern, int position, unsigned tile);
    bool canShift(const Pattern& pattern, unsigned tile, Direction dir) const;
    Pattern shiftCell(Pattern next, unsigned tile, Direction dir);
};

#endif  // PATTERN_H