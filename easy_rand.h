#ifndef EASY_RAND_H
#define EASY_RAND_H

#include <random>

static std::random_device rd;
static std::mt19937 mt(rd());

int randInRange(int min, int max);

#endif
