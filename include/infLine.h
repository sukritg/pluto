#ifndef INF_LINE_H
#define INF_LINE_H


#include "FEMEngine.h"
#include "matrix.h"

struct responseILD
{
    std::vector<double> shear;
    std::vector<double> moment;
};


class infLine
{
    public:
       infLine(std::vector<node> _nodeData,
                   std::vector<element> _elementData,
                   std::vector<bConditions> _bcData,
                   std::vector<int> _elementILD);
        ~infLine();
         void run();

    protected:

    private:
       std::vector <node> nodeData;
       std::vector <element> elementData;
       std::vector <bConditions> bcData;
       std::vector <lCombination> lcData;

       std::vector<int> elementILD; // Give the elements in an order
       FEMEngine FE;
       int parts;

      matrix infV, infM;



};

#endif // INF_LINE_H
