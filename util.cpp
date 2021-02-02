#include "util.h"

// Inclusive at both ends
int randInRange(int min, int max)
{
    std::uniform_int_distribution myDist{min, max};
    return myDist(mt);
}
