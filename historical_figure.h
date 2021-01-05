#ifndef HISTORICAL_FIGURE_H

#include <string>
#include "calendar.h"

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
    static int count;

    int         id;
    std::string name;
    Date        birthDay; // Technically doesn't hold a day yet
    int         age; // in years

    PoliticalStrategy politicalStrategy;
    AppointStrategy   appointStrategy;

    int prestige;
    
    HistoricalFigure();
    HistoricalFigure(const std::string &name);
    
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
