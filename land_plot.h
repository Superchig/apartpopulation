#ifndef LAND_PLOT_H
#define LAND_PLOT_H

#include <vector>
#include "historical_figure.h"
#include "text_renderer.h"

class Family;
class LandPlot;
class Grid;

class Family
{
public:
    HistoricalFigure *head;
    std::vector<HistoricalFigure *> members;
    LandPlot *plot = nullptr;
};

class LandPlot
{
public:
    int mapX;
    int mapY;
    
    std::vector<Family *> families;
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
