#include "NodalResponse.h"

NodalResponse::NodalResponse()
{

}

NodalResponse::~NodalResponse()
{

}

void NodalResponse::setDsp(std::vector<double> _displacement)
{
    displacement.clear();
    for (unsigned int i=0; i< _displacement.size(); i++)
    {
        displacement.push_back(_displacement[i]);
    }
}
void NodalResponse::setReac (std::vector<double> _reaction)
{
    reaction.clear();
    for (unsigned int i=0; i< _reaction.size(); i++)
    {
        reaction.push_back(_reaction[i]);
    }
}


std::vector<double> NodalResponse::getDsp()
{
    return displacement;
}

    std::vector<double> NodalResponse::getReac ()
    {
        return reaction;
    }
