#include <unordered_set>
#include <vector>

#include "../include/BoardRaw.h"

class ForbiddenWords {
    int maxDepth;
    std::unordered_set<std::string> getForbiddenWords(BoardRaw startBoard, int maxDepth);
    bool validateDuplicateStrings(std::unordered_set<std::string> &strings);
    int validateDuplicateStrings(BoardRaw startBoard, std::unordered_set<std::string> &strings);
public:
    ForbiddenWords(int maxDepth);
    std::unordered_set<std::string> getForbiddenWords();
};
