#pragma once
#include "section.h"
#include <vector>

enum class secPropType
{
    GIRDERONLY,
    COMPOSITE_SHORT,
    COMPOSITE_LONG,
    COMPOSITE_RF
};

class sectionGroup
{
private:
    std::vector<section> data;
public:
    // Data Vector Specification
    // By default : 1 - GIRDERONLY Property
    //              2 - COMPOSITE SHORT TERM Property
    //              3 - COMPOSITE LONG TERM Property
    //              4 - COMPOSITE REINFORCEMENT Property
    sectionGroup(std::vector<section> _data)
    {
        data.clear();
        data = _data;
    }

    section getProperty(secPropType type)
    {
        switch(type)
        {
        case secPropType::GIRDERONLY:
            {
                return data[0];
            }
        case secPropType::COMPOSITE_SHORT:
            {
                return data[1];
            }
        case secPropType::COMPOSITE_LONG:
            {
                return data[2];
            }
        case secPropType::COMPOSITE_RF:
            {
                return data[3];
            }

        }

    }

    ~sectionGroup()
    {

    }

};
