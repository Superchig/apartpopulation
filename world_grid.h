#ifndef WORLD_GRID_H
#define WORLD_GRID_H

#include <vector>
#include "text_renderer.h"
#include "entity.h"

class Node;
class WorldGrid;

class Node
{
  public:
    int m_x;
    int m_y;

    float hCost;
    float gCost;

    bool  occupied = false;
    bool  walkable;
    Node *parent;

    std::vector<Node*> neighbors;
    std::vector<Thing*> occupants;

    float fCost() { return gCost + hCost; }
};

class WorldGrid
{
  public:
    int                   rows;
    int                   cols;
    std::vector<Node*>     nodes;
    bool                  isActive;

    void      draw();
};

#endif