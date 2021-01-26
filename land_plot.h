#ifndef LAND_PLOT_H
#define LAND_PLOT_H

#include <vector>
#include "historical_figure.h"
#include "text_renderer.h"

class FamilyNode;
class LandPlot;
class Grid;

class FamilyNode
{
public:
    HistoricalFigure *        head;
    FamilyNode *              leader;
    std::vector<FamilyNode *> orbit;
    LandPlot *                plot;
    bool                      defunct;

    FamilyNode(FamilyNode *leader, HistoricalFigure *head, LandPlot *plot);
};

class LandPlot
{
public:
    int mapX;
    int mapY;
    
    std::vector<FamilyNode *> rootFamilies;
};

class Grid
{
public:
    int rows;
    int cols;
    std::vector<LandPlot> land;
    TextRenderer *textRen;

    Grid(TextRenderer *textRen, int rows, int cols);
    
    LandPlot &plot(int x, int y);
    void draw();
};

#endif
