#ifndef LAND_PLOT_H
#define LAND_PLOT_H

#include <vector>
#include "historical_figure.h"
#include "text_renderer.h"

class FamilyNode;
class LandPlot;
class Grid;
class Clan;

class FamilyNode
{
public:
    HistoricalFigure *        head;
    FamilyNode *              leader;
    std::vector<FamilyNode *> orbit;
    LandPlot *                plot;
    Clan *                    clan;
    
    FamilyNode(FamilyNode *leader, HistoricalFigure *head, LandPlot *plot, Clan *clan);
    
    void propagateRelocation(LandPlot *toLocation);
    size_t calcSize();
};

class Clan
{
public:
    std::vector<FamilyNode *> rootFamilies;
};

class LandPlot
{
public:
    int mapX;
    int mapY;
    
    std::vector<FamilyNode *> rootFamilies;
    
    std::string getCoords() const;
    void gatherPopulationInto(std::vector<HistoricalFigure *> &figures);
    size_t calcPopSize();
};

class Grid
{
public:
    int rows;
    int cols;
    std::vector<LandPlot> land;
    TextRenderer *textRen;
    bool isActive;

    Grid(TextRenderer *textRen, int rows, int cols);
    
    LandPlot &plot(int x, int y);
    void draw();
};

#endif
