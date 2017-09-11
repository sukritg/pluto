#pragma once
#include <vector>
#include "node.h"
#include "material.h"
#include "section.h"


class element
{
    private:
    int elementID;
    std::vector<node> nodes;
    material mate;
    section sec;

    public:
    void setProperty(std::vector<node> _nodes, material _mate, section _sec)
    {
        nodes = _nodes;
        mate = _mate;
        sec = _sec;
    }
    material getMaterial()
    {
        return mate;
    }
    section getSection()
    {
        return sec;
    }
    std::vector<node> getNodes()
    {
        return nodes;
    }
    int getID()
    {
        return elementID;
    }

    void setID(int id)
    {
        elementID = id;
    }

    void displayProperty()
    {
        std::cout << std::setw(5) << elementID << std::setw(10) << nodes[0].getID() << std::setw(10) << nodes[1].getID() << std::endl;
    }
};
