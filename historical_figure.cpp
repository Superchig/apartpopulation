#include "historical_figure.h"

#include <random>
#include "game.h"

static std::random_device rd;
static std::mt19937 mt(rd());

int HistoricalFigure::count = 0;

int randSkill();
int randInRange(int min, int max);

HistoricalFigure::HistoricalFigure(const std::string &name)
    : id(++count), name(name), birthDay(Game::main.date), age(0),  prestige(0), 
      politicalStrategy(Wastrel), appointStrategy(Random), parent1(nullptr), parent2(nullptr),
      spouse(nullptr)
{
    this->art = randSkill();
    this->intrigue = randSkill();
    this->tactics = randSkill();
    this->logistics = randSkill();
    this->inspiration = randSkill();
    
    int coinFlip = randInRange(0, 1);
    this->sex = coinFlip == 1 ? Male : Female;
}

HistoricalFigure::HistoricalFigure() : HistoricalFigure("Pre-Default Name")
{
    this->name = "Noble " + std::to_string(this->id);
}

HistoricalFigure::HistoricalFigure(int age) : HistoricalFigure()
{
    this->age = age;
}

HistoricalFigure::HistoricalFigure(const std::string& name, int age) : HistoricalFigure(name)
{
    this->age = age;
}


std::string HistoricalFigure::getSpouseName() const
{
    return spouse == nullptr ? "No Spouse" : spouse->name;
}


int randSkill()
{
    return randInRange(1, 6) * randInRange(1, 6) * randInRange(1, 6);
}

int randInRange(int min, int max)
{
    std::uniform_int_distribution myDist{min, max};
    return myDist(mt);
}
