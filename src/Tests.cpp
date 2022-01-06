#include "../include/Tests.h"
#include "../include/BoardRep.h"
#include "../include/BoardRaw.h"
#include "../include/Util.h"
#include "../include/ForbiddenWordsUtil.h"

void TestPaths();

int runTests() {
    TestPaths();
    std::vector<int> g = {
        24, 23, 22, 21, 20,
        19, 18, 17, 16, 15,
        14, 13, 12, 11, 10,
        9,  8 ,  7,  6,  5,
        4,   3,  2,  1,  0
    };
    auto b = BoardRaw{g, 5, 5};
    auto boardRep = BoardRep{b};
    auto b2 = boardRep.toBoard();
    DEBUG(b2);
    assertm(b == b2, "should be equal");

    DEBUG("all tests passed");
    return 0;
}

void TestPaths() {
    std::string path = "llrududrl";
    auto compressedPath = CompressedPath(path);
    auto decPath = compressedPath.decompress();
    assertm(path == decPath, "dec should be the same");
}