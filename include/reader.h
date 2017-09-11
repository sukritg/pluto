#ifndef READER_H
#define READER_H
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <map>
#include <iomanip>
#include <sstream>

class reader
{
    public:
        //Constructor
        reader(std::string _fileName);
        //Destructor
        ~reader();
        //Populate the reservedKeys from keyConfig.dat file
        void configKeys();
        void printKeys();

        //Lexer
        void lexer();
        //print keys and values of tokenList
        void printTokenList();

        //Check for item in a vector of strings
        template <class V>
        bool checkItem(std::vector<V> _list,V _item);

        //Get token list
        std::map<std::string,std::vector<double>> getTokens();
        //Get module name
        std::string getModuleName();
    protected:
    private:
        std::string moduleName;
        std::string fileContents;
        std::vector<std::string> reservedKeys;
        std::map<std::string,std::vector<double>> tokenList;



};

template <typename T>
double toDouble(T dat)
{
    double res;
    std::stringstream ss;
    ss << std::setprecision(3) << dat;
    ss >> std::setprecision(3) >> res;
    ss.clear();
    return res;
}

#endif // READER_H
