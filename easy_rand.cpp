#include "easy_rand.h"

int randInRange(int min, int max)
{
    std::uniform_int_distribution myDist{min, max};
    return myDist(mt);
}
