#pragma once
#include <vector>

class bConditions
{
    private:
    int nodeID;
    std::vector<int> boundary;

    public:
    bConditions()
    {
        nodeID = 0;
        boundary = {0, 0, 0, 0, 0, 0};
    }
    ~bConditions()
    {

    }
    void setProperty(int _nodeID, std::vector<int> _boundary)
    {
        nodeID = _nodeID;
        boundary = _boundary;
    }
    std::vector<int> getProperty()
    {
        return boundary;
    }
    int getID()
    {
    return nodeID;
    }
    void displayProperty()
    {
   std::cout << std::setw(5) << nodeID << std::setw(5) << boundary[0]
   << std::setw(5) << boundary[1]
   << std::setw(5) << boundary[2]
   << std::setw(5) << boundary[3]
   << std::setw(5) << boundary[4]
   << std::setw(5) << boundary[5]
   << std::endl;
    }

};



