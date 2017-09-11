#ifndef ELEMENTRESPONSE_H
#define ELEMENTRESPONSE_H
#include <vector>

class ElementResponse
{
    public:
        ElementResponse();
        ~ElementResponse();
        void setID( int _id);
        int getID();
        void setNodes(int _SN, int _EN);
        int getSNode();
        int getENode();

        void setProperty(std::vector<double> _force);
        std::vector<double> getProperty();
    protected:

    private:
        int SN, EN, ID;
        std::vector<double> force;

};

#endif // ELEMENTRESPONSE_H
