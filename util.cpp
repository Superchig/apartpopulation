#include "util.h"

int randInRange(int min, int max)
{
    std::uniform_int_distribution myDist{min, max};
    return myDist(mt);
}
