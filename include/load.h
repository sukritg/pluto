#pragma once
#include <vector>

enum class loadType
{
    EC_Fy,
    EU_Fy
};

class load
{
private:
    int loadID;
    loadType type;
    std::vector<double> values;
    std::vector<int> elID;
public:
    load(int _loadID, loadType _type,std::vector<int> _elID)
    {
        elID.clear();
        loadID = _loadID;
        elID = _elID;
        type = _type;
    }

    load(int _loadID, loadType _type,int _elID)
    {
        elID.clear();
        loadID = _loadID;
        elID.push_back(_elID);
        type = _type;
    }

    void setProperty(std::vector<double> &_values)
    {
        values = _values;
    }




    std::vector <int> getElements()
    {
        return elID;
    }

    std::vector<double> getValues()
    {
        return values;
    }

    int getID()
    {
        return loadID;
    }

    loadType getType()
    {
        return type;
    }




    void displayProperty()
    {
        switch (type)
        {
        case loadType::EC_Fy :
            {
                std::cout << "Load Type - Element Concentrated Load - Y " << std::endl;
                std::cout << "Element - ";
                for (unsigned int i=0; i < elID.size();i++)
                    std::cout << elID[i] << "\t";
                std::cout << std::endl;
                std::cout << "Intensity = " << values[0] << "\t Local distance from left = " << values[1] << std::endl;
                break;
            }
        case loadType::EU_Fy :
            {
                std::cout << "Load Type - Element Uniformly Distributed Load - Y " << std::endl;
                std::cout << "Element - ";
                for (unsigned int i=0; i < elID.size();i++)
                    std::cout << elID[i] << "\t";
                std::cout << std::endl;
                if (values.size()==1)
                    std::cout << "Intensity = " << values[0] << std::endl;
                else
                    std::cout << "Intensity = " << values[0] << std::endl;
                    std::cout << "Local distance from left =  " << values[1] << std::endl;
                    std::cout << "Local distance from right = " << values[2] << std::endl;
                break;

                break;
            }
        }

    }
};
