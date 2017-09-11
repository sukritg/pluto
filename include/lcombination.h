#pragma once
#include "load.h"

class lCombination
{
private:
    int lcID;
    std::vector<load> loads;
public:
    lCombination(int _lcID)
    {
        lcID = _lcID;
    }
    ~lCombination()
    {

    }
    void setProperty(std::vector<load> _loads)
    {
        loads = _loads;
    }
    std::vector<load> getProperty()
    {
        return loads;
    }
    int getID()
    {
        return lcID;
    }
    void displayProperty()
    {
        std::cout << "--------------------------------------------------" << std::endl;
        std::cout << "Load Combination - " << lcID << std::endl;
        std::cout << "--------------------------------------------------" << std::endl;
        for (unsigned int i=0; i<loads.size();i++)
        {
            loads[i].displayProperty();

        }

    }
};
