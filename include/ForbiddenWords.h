#include <unordered_set>
#include <vector>

#include "../include/BoardRaw.h"

class ForbiddenWords {
    int maxDepth, width, height;
    std::unordered_set<std::string> getForbiddenWords(BoardRaw startBoard, int maxDepth);
    bool validateDuplicateStrings(std::unordered_set<std::string> &strings);
    int validateDuplicateStrings(BoardRaw startBoard, std::unordered_set<std::string> &strings);
    int removeDuplicateSuffixes(std::unordered_set<std::string> &strings);

public:
    ForbiddenWords(int maxDepth, int width, int height);
    std::unordered_set<std::string> getForbiddenWords();
};
