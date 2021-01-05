#include "historical_figure.h"

#include <random>
#include "game.h"

static std::random_device rd;
static std::mt19937 mt(rd());
static std::uniform_int_distribution dist(1, 6);

int HistoricalFigure::count = 0;

int randSkill();

HistoricalFigure::HistoricalFigure(const std::string &name)
    : id(++count), name(name), birthDay(Game::main.date), age(0),
      politicalStrategy(Wastrel), appointStrategy(Random),
      prestige(0)
{
    this->art = randSkill();
    this->intrigue = randSkill();
    this->tactics = randSkill();
    this->logistics = randSkill();
    this->inspiration = randSkill();
}

HistoricalFigure::HistoricalFigure() : HistoricalFigure("Pre-Default Name")
{
    this->name = "Noble " + std::to_string(this->id);
}

int randSkill()
{
    return dist(mt) * dist(mt) * dist(mt);
}
