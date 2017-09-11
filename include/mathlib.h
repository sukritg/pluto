
#ifndef MATHLIB_H
#define MATHLIB_H

#include <iostream>
#include <cmath>
#include "matrix.h"
#include <string>

class mathlib
{
public:

mathlib()
{

}

~mathlib()
{

}
matrix idmatrix(int size)
{
    matrix temp(size,size);
    for (int i=1;i<=size; i++)
    {
        temp(i,i) = 1;
    }

    return temp;
}

matrix idmatrix(int size,std::string name)
{
    matrix temp(size,size,name);

    for (int i=1;i<=size; i++)
    {
        temp(i,i) = 1;
    }

    return temp;
}

void rowMultiply(matrix &mat,unsigned int rowN, double var)
{
    if (!mat.isEmpty() && rowN<=mat.rowSize())
    {
        for (unsigned int i=1; i<=mat.colSize(); i++)
            mat(rowN,i) = mat(rowN,i) * var;
    }
}

void colMultiply(matrix &mat,unsigned int colN, double var)
{
    if (!mat.isEmpty() && colN<mat.colSize())
    {
        for (unsigned int i=1; i<=mat.rowSize(); i++)
            mat(i,colN) = mat(i,colN) * var;
    }
}

matrix gaussSolve(matrix &A, matrix &B)
{
    // Check
    if (A.rowSize()!= B.rowSize())
        std::cout << "The system of linear equations does not have an unique solution." << std::endl;
    if (B.colSize() > 1)
        std::cout << "Inappropriate column size of right hand matrix" << std::endl;

    // Set output matrix size
    matrix T(A.rowSize(),A.colSize()+1);    // Augmented Matrix
    matrix x(A.rowSize(),1);                // Result Matrix

    for (unsigned int i = 1; i <= T.rowSize(); i++)
    {
         for (unsigned int j = 1; j <=T.colSize(); j++)
        {
            if (j>A.colSize())
                T(i,j) = B(i,1);
            else
                T(i,j) = A(i,j);
        }
    }

    double maxRow, maxElem,tmp;
    for (unsigned int i = 1 ; i<=T.rowSize();i++)
    {
        // Finding maximum value in the column
        maxRow = i;
        maxElem = T(i,i);

        for (unsigned int j = i+1; j<=T.rowSize(); j++)
        {
            if (maxElem < std::abs(T(j,i)))
            {
                maxRow = j; maxElem = T(j,i);
            }
        }

        // Swap current row with row with maximum value in the column
            for (unsigned int j=1; j<=T.colSize(); j++)
            {
            tmp = T(maxRow,j);
            T(maxRow,j) = T(i,j) ;
            T(i,j) = tmp;
            }

        if (T(i,i)==0) {std::cout << "Matrix is singular"<<std::endl;}

        // Pivoting the current row
        tmp = 1/T(i,i);
        rowMultiply(T,i,tmp);
        for (unsigned int j=i+1; j<=T.rowSize();j++)
        {
            tmp = -T(j,i);
            for (unsigned int k=1; k<=T.colSize(); k++)
            {
                T(j,k) = T(j,k) + tmp * T(i,k);
            }
        }
    }
     // Solving Ax = b by backward substitution
     for (int i=T.rowSize();i>0;i--)
     {
         x(i,1) = T(i,T.colSize())/T(i,i);
         for (int j=1;j<i;j++)
         {
           T(j,T.colSize()) = T(j,T.colSize()) - x(i,1)*T(j,i);
         }
     }

return x;
}

double dotProduct(matrix &A, matrix &B)
{
    double result=0;
    //Check the dimension of the matrices
    if (A.colSize()!= B.colSize() || A.rowSize() != B.rowSize())
        std::cout << "Vectors with equal dimension necessary for the operation" << std::endl;
    if (A.colSize() == 1)
        {
            result = 0;
            for (unsigned int i = 1; i<=A.rowSize();i++)
                result = result + A(i,1)*B(i,1);
        return result;
        }
    else if (A.rowSize() == 1)
    {
        result = 0;
        for (unsigned int i = 1; i<=A.colSize(); i++)
            result = result + A(1,i)*B(1,i);
    return result;
    }
    else
        return result;
}


matrix choleskySolve(matrix &A, matrix &B)
{
      // Check
    if (A.rowSize()!= B.rowSize())
        std::cout << "The system of linear equations does not have an unique solution." << std::endl;
    if (B.colSize() > 1)
        std::cout << "Inappropriate column size of right hand matrix" << std::endl;

    // Check for positive definiteness


    // Set output matrix size
    matrix L(A.rowSize(),A.colSize());    // Augmented Matrix
    matrix x(A.rowSize(),1);              // Result Matrix
    double temp;

    for (unsigned int i=1; i <= A.rowSize(); i++)
    {
        temp = 0;

        for (unsigned int j=1; j <= i; j++)
        {
            if (i==j)
               {
                    temp = 0;
                    for (unsigned int k=1; k < i; k++)
                    {
                        temp = temp + pow(L(i,k),2.0);
                    }
                    L(i,j) = sqrt(A(i,i) - temp);
               }
               else
               {
                   temp = 0;
                   for (unsigned int k=1; k < i; k++)
                   {
                       temp = temp + L(i,k)*L(j,k);
                   }
                   L(i,j) = (1/L(j,j))*(A(i,j) - temp);
               }
        }
    }

    // Solving Ly = b by forward substitution
    matrix y = x;
    y.setValue(0.0);
     for (unsigned int i=1; i<=L.rowSize(); i++)
     {
         temp = 0;
         for (unsigned int j=1; j<i; j++)
         {
           temp = temp + y(j,1) * L(i,j);
         }
         y(i,1) = (B(i,1)-temp)/L(i,i);
     }

    // Solving L'x = y by backward substitution
    L = ~L;

    for (unsigned int i=L.rowSize(); i >= 1; i--)
     {
         temp = 0;
         for (unsigned int j=i+1; j <= L.colSize(); j++)
         {
           temp = temp + x(j,1) * L(i,j);
          }
         x(i,1) = (y(i,1) - temp)/L(i,i);
     }
    return x;
}

matrix luSolve(matrix &A, matrix &B)
{
    // Check
    if (A.rowSize()!= B.rowSize())
        std::cout << "The system of linear equations does not have an unique solution." << std::endl;
    if (B.colSize() > 1)
        std::cout << "Inappropriate column size of right hand matrix" << std::endl;

    // Set output matrix size
    matrix x(A.rowSize(),1);                // Result Matrix
    matrix U = A;                           // Upper Triangular Matrix
    matrix L(A.rowSize(),A.colSize());      // Lower Triangular Matrix


    double maxRow, maxElem,tmp;
    for (unsigned int i = 1 ; i<=A.rowSize();i++)
    {
        // Finding maximum value in the column
        maxRow = i;
        maxElem = U(i,i);

        for (unsigned int j = i+1; j<=U.rowSize(); j++)
        {
            if (maxElem < std::abs(U(j,i)))
            {
                maxRow = j; maxElem = U(j,i);
            }
        }

        // Swap current row with row with maximum value in the column
            for (unsigned int j=1; j<=U.colSize(); j++)
            {
            tmp = U(maxRow,j);
            U(maxRow,j) = U(i,j) ;
            U(i,j) = tmp;
            }

        if (U(i,i)==0) {std::cout << "Matrix is singular"<<std::endl;}

        // Pivoting the current row
        tmp = 1/U(i,i);
        //rowMultiply(U,i,tmp);
        for (unsigned int j=i+1; j<=U.rowSize();j++)
        {
            tmp = -U(j,i);
            for (unsigned int k=1; k<=U.colSize(); k++)
            {

                U(j,k) = U(j,k) + tmp * U(i,k)/U(i,i);
            }
        }
    }

    for (unsigned int i=1; i<=L.rowSize(); i++)
    {
        for(unsigned int j=i; j<=L.colSize(); j++)
        {
            if (i==j)
                L(i,j) = 1;
            else
                L(j,i) = U(i,j)/U(i,i);
        }
    }

    // Solving Ly = b by forward substitution
    matrix y = x;
    y.setValue(0.0);
    double temp;
     for (unsigned int i=1; i<=L.rowSize(); i++)
     {
         temp = 0;
         for (unsigned int j=1; j<i; j++)
         {
           temp = temp + y(j,1) * L(i,j);
         }
         y(i,1) = (B(i,1)-temp)/L(i,i);
     }

    // Solving Ux = y by backward substitution

    for (unsigned int i=U.rowSize(); i >= 1; i--)
     {
         temp = 0;
         for (unsigned int j=i+1; j <= U.colSize(); j++)
         {
           temp = temp + x(j,1) * U(i,j);
          }
         x(i,1) = (y(i,1) - temp)/U(i,i);
     }
    return x;
}
};
#endif // MATHLIB_H
