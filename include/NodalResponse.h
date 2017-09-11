#ifndef NODALRESPONSE_H
#define NODALRESPONSE_H

#include <vector>

class NodalResponse
{
    public:
        NodalResponse();
        ~NodalResponse();
        void setDsp  (std::vector<double> _displacement);
        void setReac (std::vector<double> _reaction);
        std::vector<double> getDsp ();
        std::vector<double> getReac ();
    protected:

    private:
        std::vector <double> displacement;
        std::vector <double> reaction;
};

#endif // NODALRESPONSE_H
