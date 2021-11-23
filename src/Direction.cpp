#include "../include/Direction.h"

std::ostream& operator<<(std::ostream& out, const Direction& dir) {
    switch (dir) {
        case Direction::U:
            out << "U";
            break;
        case Direction::R:
            out << "R";
            break;
        case Direction::D:
            out << "D";
            break;
        default:
            out << "L";
            break;
    }
    return out;
}