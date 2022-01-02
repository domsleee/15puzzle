#ifndef WALKINGDISTANCE_H
#define WALKINGDISTANCE_H

#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace WalkingDistance {

using Cost = uint_fast8_t;
using Index = uint_fast32_t;
using RowColType = uint_fast8_t;

extern std::vector<Cost> costs;
extern std::vector<Index> edgesUp;
extern std::vector<Index> edgesDown;
extern std::vector<RowColType> row;
extern std::vector<RowColType> col;
extern int width;
extern int height;

void load(const std::vector<int>& goal, int width, int height);

int getIndex(const std::vector<int>& grid, bool alongRow = true);

};  // namespace WalkingDistance

#endif  // WALKINGDISTANCE_H