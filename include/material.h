#pragma once
#include <vector>

class material
{
    private:
    int materialID;
    double E;

    public:
    void setProperty(std::vector<double> &data)
    {
        E = data[0];
    }
    std::vector<double> getProperty()
    {
        std::vector<double> data;
        data.push_back(E);
        return data;
    }

    int getID()
    {
        return materialID;
    }

    void setID(int id)
    {
        materialID = id;
    }
};
