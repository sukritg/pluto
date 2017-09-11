# pragma once
# include <vector>
#include <iostream>
#include <iomanip>
class node
{
    private:
    int nodeID;
    std::vector <double> coordinates;

    public:

    void setProperty(double x,double y)
    {
        coordinates.clear();
        coordinates.push_back(x);
        coordinates.push_back(y);
     }

    std::vector<double> getProperty()
    {
        return coordinates;
    }


    int getID()
    {
        return nodeID;
    }

    void setID(int id)
    {
        nodeID = id;
    }

    void displayProperty()
    {
        std::cout << std::setw(5) << nodeID << std::setw(10) << coordinates[0] << std::setw(10) << coordinates[1] << std::endl;
    }
};
