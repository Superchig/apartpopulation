#ifndef UTIL_H
#define UTIL_H

#include <random>
#include <algorithm>

static std::random_device rd;
static std::mt19937 mt(rd());

int randInRange(int min, int max);

template<class T> void removeItem(std::vector<T> &vec, T item)
{
    auto it = std::find(std::begin(vec), std::end(vec), item);
    assert(it != std::end(vec));
    vec.erase(it);
}

#endif
