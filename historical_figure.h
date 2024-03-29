#ifndef HISTORICAL_FIGURE_H
#define HISTORICAL_FIGURE_H

#include <string>
#include <vector>
#include "calendar.h"

class FamilyNode;

enum PoliticalStrategy
{
    Wastrel, Warrior, Bureaucrat, Artist, Factioneer
};

enum AppointStrategy
{
    Skill, Faction, Loyalty, Random
};

enum Sex
{
    Male, Female
};

static std::string sexStrings[] = { "M", "F" };

class HistoricalFigure
{
  public:
    static int count;

    int         id;
    std::string name;
    Date        birthDay; // Technically doesn't hold a day yet
    int         age; // in years
    Sex         sex;
    int         prestige;
    int         desiredKids;
    
    bool isAlive;

    PoliticalStrategy politicalStrategy;
    AppointStrategy   appointStrategy;
    
    HistoricalFigure *parent1;
    HistoricalFigure *parent2;
    HistoricalFigure *spouse;

    std::vector<HistoricalFigure *> kids;

    FamilyNode *family;
    
    HistoricalFigure();
    HistoricalFigure(int age);
    HistoricalFigure(const std::string &name);
    HistoricalFigure(const std::string &name, int age);
    
    std::string getSpouseName() const;
    
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
