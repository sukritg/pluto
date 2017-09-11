#include "ElementResponse.h"

ElementResponse::ElementResponse()
{

}

ElementResponse::~ElementResponse()
{
    //dtor
}

void ElementResponse::setProperty(std::vector<double> _force)
{
    force.clear();
    for (unsigned int i = 0; i < _force.size(); i ++)
        force.push_back(_force[i]);
}

std::vector<double> ElementResponse::getProperty()
{
    return force;
}
 void ElementResponse::setID( int _id)
 {
     ID = _id;
 }

  int ElementResponse::getID()
  {
      return ID;
  }

  void ElementResponse::setNodes(int _SN, int _EN)
  {
      SN = _SN; EN = _EN;
  }

  int ElementResponse::getSNode()
  {
      return SN;
  }
  int ElementResponse::getENode()
  {
    return EN;
  }

