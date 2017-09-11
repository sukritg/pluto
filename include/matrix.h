#ifndef MATRIX_H
#define MATRIX_H

#include <string>
#include <iostream>
#include <vector>

class matrix
{
    public:
        matrix();
        matrix(unsigned int _rows, unsigned int _cols);
        matrix(unsigned int _rows, unsigned int _cols, std::string _name);
        matrix(const matrix &);
        ~matrix();

        unsigned int rowSize() const;
        unsigned int colSize() const;
        std::string getName() const;
        void setName(std::string _name);
        void setSize(unsigned int _rows, unsigned int _cols);
        void setValue(double _value);

        void errorHandler(int _errorCode);
        void errorHandler(int _errorCode) const;
        bool isEmpty();
        bool isEmpty() const;

        matrix & operator= (const matrix &mat);

        double & operator() (unsigned int _row, unsigned int _col);
        double & operator() (unsigned int _row, unsigned int _col) const;
		matrix operator() (std::vector<int> rowvec, std::vector<int> colvec);


        matrix operator+ (const matrix &rhs);
        matrix operator- (const matrix &rhs);
        matrix operator* (const matrix &rhs);
        matrix operator~ ();
    protected:


    private:
        double **data;
        unsigned int rows, cols;
        std::string name;
};



#endif // MATRIX_H
