#ifndef INF_LINE_H
#define INF_LINE_H


#include "FEMEngine.h"
#include "matrix.h"

struct HL93
{
    double w_1 = 32.0, w_2 = 32.0, w_3 = 8.0;
    double d_1 = 0.0, d_2 = 14.0, d_3 = 14.0;
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
