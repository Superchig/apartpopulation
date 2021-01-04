#ifndef HISTORICAL_FIGURE_H

#include <string>

enum PoliticalStrategy
{
    Wastrel, Warrior, Bureaucrat, Artist, Factioneer
};

enum AppointStrategy
{
    Skill, Faction, Loyalty, Random
};

class HistoricalFigure
{
  public:
    std::string name;
    int         id;

    PoliticalStrategy politicalStrategy;
    AppointStrategy   appointStrategy;

    int prestige;

#pragma region Skills
    int art;
    int intrigue;

    // Military skills
    int tactics;
    int logistics;
    int inspiration;
#pragma endregion
};

#endif
