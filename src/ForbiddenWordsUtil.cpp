
#include "../include/ForbiddenWordsUtil.h"


ValidationRet::ValidationRet(int minBfsLength, int blankLocation, const std::vector<std::string> &stringsLessThanLength)
    : minBfsLength(minBfsLength),
      blankLocation(blankLocation),
      stringsLessThanLength(stringsLessThanLength)
    {}


