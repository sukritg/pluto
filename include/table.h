#ifndef TABLE_H
#define TABLE_H

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>

template <typename T>
std::string toString(T dat)
{
    std::string res;
    std::stringstream ss;
    ss << std::scientific << std::setprecision(3) << dat;
    ss>>res;
    ss.clear();
    return res;
}

template <typename T>
std::string toString(T dat, double tol)
{
    std::string res;
    std::stringstream ss;
    if (std::abs(dat) < tol){dat = 0.0;}
    ss << std::scientific << std::setprecision(3) << dat;
    ss>>res;
    ss.clear();
    return res;
}
template <typename T>
std::string toString(T dat, double tol, bool sci)
{
    std::string res;
    std::stringstream ss;
    if (std::abs(dat) < tol){dat = 0.0;}
    if (sci == true)
        ss << std::scientific << std::setprecision(2) << dat;
    else
        ss << std::fixed << std::setprecision(2) << dat;
    ss>>res;
    ss.clear();
    return res;
}

struct rowData
{
    std::vector<std::string> rdata;
    void push(std::string dat)
    {
        rdata.push_back(dat);
    }
    std::string pull(unsigned int pos)
    {
        if (pos >= rdata.size())
        {
            std::cout << "Data position out of bounds" << std::endl;
            return " ";
        }
            return rdata[pos];
    }
    void clear()
    {
        rdata.clear();
    }
    unsigned int size()
    {
        return rdata.size();
    }
};



class table
{
    public:
        table();
        ~table();
        table(const table& other);

        void setTable(std::vector<std::string> &_tableHeader, std::vector<rowData> &_data, std::vector<int> &_colSize, std::vector<int> &_align);
        void getTable(std::vector<std::string> &_tableHeader, std::vector<rowData> &_data, std::vector<int> &_colSize, std::vector<int> &_align);
        void reverseData();
    protected:

    private:
        std::vector<std::string> tableHeader;
        std::vector<rowData> data;
        std::vector<int> colSize;
        std::vector<int> align; //Left = 0, Right = 1;
        std::vector<int> precision;

};

#endif // TABLE_H
