#include <unordered_set>
#include <vector>

#include "../include/BoardRaw.h"
#include "../include/ForbiddenWordsUtil.h"
#include "../include/Util.h"

class ForbiddenWords {
    int maxDepth, width, height;
    std::unordered_set<std::string> getForbiddenWords(BoardRaw startBoard, int maxDepth);
    bool validateDuplicateStrings(std::unordered_set<std::string> &strings);
    int validateDuplicateStrings(BoardRaw startBoard, std::unordered_set<std::string> &strings);

public:
    int removeDuplicateSuffixes(std::unordered_set<std::string> &strings);
    ForbiddenWords(int maxDepth, int width, int height);
    std::unordered_set<std::string> getForbiddenWords();
    void printMessage() const {
        DEBUG("Getting forbidden words with depth: " << maxDepth);
    }
};
