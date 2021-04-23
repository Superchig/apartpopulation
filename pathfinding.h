#pragma once
#include "world_grid.h"
#include <iostream>

auto Distance = [](Node *a, Node *b) {
    return sqrtf((float)((a->m_x - b->m_x) * (a->m_x - b->m_x) +
                         (a->m_y - b->m_y) * (a->m_y - b->m_y)));
};

class Pathfinder
{
  public:
    std::list<Node *> FindPath(Node *start, Node *end)
    {

        std::list<Node *> OpenNodes;
        std::list<Node *> ClosedNodes;
        Node *            current = start;

        OpenNodes.push_back(start);

        while (!OpenNodes.empty())
        {
            OpenNodes.sort([](Node *lhs, Node *rhs) {
                if (lhs->fCost() < rhs->fCost() ||
                    lhs->fCost() == rhs->fCost() && lhs->hCost < rhs->hCost)
                {
                    return true;
                }
                else
                {
                    return false;
                }
            });
            current = OpenNodes.front();

            if (end == current)
            {
                std::list<Node *> path;
                Node *            pathCurr = current;

                while (pathCurr != start)
                {

                    // std::cout << std::to_string(pathCurr->m_x) + " / " +
                    // std::to_string(pathCurr->m_y) + '\n';
                    path.push_back(pathCurr);
                    pathCurr = pathCurr->parent;
                }

                // path.push_back(start);

                path.reverse();

                return path;
            }

            OpenNodes.pop_front();
            ClosedNodes.push_back(current);

            for (auto nodeNeighbor : current->neighbors)
            {

                bool visited =
                    (std::find(ClosedNodes.begin(), ClosedNodes.end(),
                               nodeNeighbor) != ClosedNodes.end());
                bool alreadyOpen =
                    (std::find(OpenNodes.begin(), OpenNodes.end(),
                               nodeNeighbor) != OpenNodes.end());

                if (!alreadyOpen && !visited && nodeNeighbor->walkable &&
                    !nodeNeighbor->occupied)
                {
                    OpenNodes.push_back(nodeNeighbor);

                    float newCostToNeighbor =
                        current->gCost + Distance(current, nodeNeighbor);

                    if (newCostToNeighbor < nodeNeighbor->gCost || !alreadyOpen)
                    {
                        nodeNeighbor->gCost  = newCostToNeighbor;
                        nodeNeighbor->hCost  = Distance(end, nodeNeighbor);
                        nodeNeighbor->parent = current;

                        if (!alreadyOpen)
                        {
                            OpenNodes.push_back(nodeNeighbor);
                        }
                    }
                }
            }
        }
        std::list<Node *> emptyPath;
        return emptyPath;
    }
};