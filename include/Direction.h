#ifndef DIRECTION_H
#define DIRECTION_H

#include <ostream>

enum class Direction { U = 0, R, D, L };

std::ostream& operator<<(std::ostream& out, const Direction& dir);

#endif  // DIRECTION_H
