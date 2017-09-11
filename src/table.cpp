#include "table.h"

table::table()
{
    //ctor
}

table::~table()
{
    //dtor
}

table::table(const table& other)
{
    //copy ctor
}

void table::setTable(std::vector<std::string> &_tableHeader, std::vector<rowData> &_data, std::vector<int> &_colSize, std::vector<int> &_align)
{
    if (_tableHeader.size()!= _colSize.size() || _align.size()!= _colSize.size())
    {
        std::cout<<"tableHeader("<<_tableHeader.size()<<")"<<std::endl;
        std::cout<<"data (" <<_data[0].size() <<")" <<std::endl;
        std::cout<<"colSize ("<<_colSize.size()<<")"<<std::endl;
        std::cout<<"align ("<<_align.size()<<")"<<std::endl;
        std::cout << "Error in table definition" <<std::endl;
    }
    tableHeader = _tableHeader;
    data = _data;
    colSize = _colSize;
    align = _align;
}

void table::getTable(std::vector<std::string> &_tableHeader, std::vector<rowData> &_data, std::vector<int> &_colSize, std::vector<int> &_align)
{
    _tableHeader = tableHeader;
    _data = data;
    _colSize = colSize;
    _align = align;
}

  void table::reverseData()
  {
      std::reverse(data.begin(),data.end());
  }
