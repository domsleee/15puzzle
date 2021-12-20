#include "BoardRaw.h"
#include "Util.h"
#include <memory>

struct BoardRep {
    BoardRep(const BoardRaw &board);
    std::vector<uint8_t> grid;

    BoardRaw toBoard() const;

    friend bool operator==(const BoardRep &lhs, const BoardRep &rhs) {
        return lhs.grid == rhs.grid;
    }
    
    friend bool operator!=(const BoardRep &lhs, const BoardRep &rhs) {
        return !(lhs == rhs);
    }
};


template<>
struct std::hash<BoardRep> {
    std::size_t operator()(BoardRep const& boardRep) const noexcept {
        // see https://stackoverflow.com/questions/20511347/a-good-hash-function-for-a-vector
        std::size_t seed = boardRep.grid.size();
        for(auto& i : boardRep.grid) {
            seed ^= i + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};
