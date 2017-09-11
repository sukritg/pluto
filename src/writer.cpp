#include "writer.h"

writer::writer(std::string _fileName)
{
    //ctor
    fileName = _fileName;
    out.open(fileName.c_str());
    if (!out.good())
        std::cout<<"Error creating file for output"<<std::endl;
    tableNo = 1;
}

writer::~writer()
{
   close();
    //dtor
}

void writer::writeTable(table &T, std::string title)
{
    std::vector<std::string> tableHeader;
    std::vector<rowData> data;
    std::vector<int> colSize, align;
    T.getTable(tableHeader,data,colSize,align);
    out << std::left << std::endl;
    // Printing Table Title
    out << "Table " << tableNo << ": " << title << std::endl;
    tableNo++;
    // Printing Table Header

    out <<std::left << std::setw(1) << "+" ;
        for (unsigned int i = 0; i < tableHeader.size(); i++)
        {
            out << std::setw(1) << "-" ;

            out << std::right << std::setw(colSize[i]) << std::setfill('-') << "+";

        }
        out<<std::setfill(' ')<< std::endl;

    out  << std::left << std::setw(1) << "|";
    for (unsigned int i = 0; i < tableHeader.size(); i++)
    {
        if (align[i]==0)
        {
            out << std::left << std::setw(colSize[i]) << tableHeader[i] ;
        }
        else
        {
             out << std::setw(colSize[i]) << std::right << tableHeader[i] ;
        }
         out  << std::left << std::setw(1) << "|";
    }

    out << std::endl;
    out <<std::left << std::setw(1) << "+" ;
        for (unsigned int i = 0; i < tableHeader.size(); i++)
        {
            out << std::setw(1) << "-" ;

            out << std::right << std::setw(colSize[i]) << std::setfill('-') << "+";

        }
        out<<std::setfill(' ')<< std::endl;

    // Printing Table Data
    for (unsigned int i = 0; i < data.size(); i++)
    {
        out  << std::left << std::setw(1) << "|";

        for (unsigned int j = 0; j < data[i].size(); j++)
        {
            if (align[j]==0)
            {
                out << std::left << std::setw(colSize[j])  <<  data[i].pull(j);
            }
            else
            {
                out << std::right << std::setw(colSize[j]) << data[i].pull(j) ;
            }
            out <<std::setw(1) << "|";
        }
        out<<std::endl;


        out <<std::left << std::setw(1) << "+" ;
        for (unsigned int i = 0; i < tableHeader.size(); i++)
        {
            out << std::setw(1) << "-" ;

            out << std::right << std::setw(colSize[i]) << std::setfill('-') << "+";

        }
        out<<std::setfill(' ')<< std::endl;

    }


}

void writer::writeTableCopy(table &T, std::string title)
{
    std::vector<std::string> tableHeader;
    std::vector<rowData> data;
    std::vector<int> colSize, align;
    T.getTable(tableHeader,data,colSize,align);
    out << std::left << std::endl;
    // Printing Table Title
    out << "Table " << tableNo << ": " << title << std::endl;
    tableNo++;
    // Printing Table Header
    for (unsigned int i = 0; i < tableHeader.size(); i++)
        out << std::setw(colSize[i]) << std::setfill('-') << "";
    out << std::endl << std::setfill(' ');

    for (unsigned int i = 0; i < tableHeader.size(); i++)
        out << std::setw(colSize[i]) << tableHeader[i] ;
    out << std::endl;

    for (unsigned int i = 0; i < tableHeader.size(); i++)
        out << std::setw(colSize[i]) << std::setfill('-') << "";
    out << std::endl << std::setfill(' ');

    // Printing Table Data
    for (unsigned int i = 0; i < data.size(); i++)
    {
        for (unsigned int j = 0; j < data[i].size(); j++)
            out << std::setw(colSize[j]) << data[i].pull(j);
        out << std::endl;
    }
    for (unsigned int i = 0; i < tableHeader.size(); i++)
        out << std::setw(colSize[i]) << std::setfill('-') << "";
    out << std::endl << std::setfill(' ');

}
void writer::writeText (const std::string &text)
{
    out << text << std::endl;
}
void writer::close()
{
    out.close();
}
 void writer::writeMatrix (const matrix &mat, int width)
 {

    if (!mat.isEmpty())
  {
     out << "Matrix: " << mat.getName() <<std::endl;
     out << "Number of rows = " << mat.rowSize() << std::endl;
     out << "Number of columns = " << mat.colSize() << std::endl;
     for (unsigned int i = 1; i<=mat.rowSize(); i++)
     {
         out << std::setw(1) << "+";
         for (unsigned int j = 1; j<=mat.colSize();j++)
            {
                out << std::setfill('-') << std::setw(width) << "";
                out << std::setw(1) << "+";
            }

         out << std::endl << std::setfill(' ');

         for (unsigned int j = 1; j<=mat.colSize(); j++)
         {
             out <<std::setw(1)<<"|";
             out << std::left << std::setw(width) << mat(i,j);
         }
         out <<std::setw(1)<<"|";
         out << std::endl;
     }
      out << std::setw(1) << "+";
     for (unsigned int j = 1; j<=mat.colSize();j++)
            {
                out << std::setfill('-') << std::setw(width) << "";
                out << std::setw(1) << "+";
            }
            out <<std::endl;
 }
     else
     {
            out << "Matrix is empty" <<std::endl;
     }
 }

 void writer::writeMatrix (const matrix &mat, int width,bool sci)
 {
     if (sci == true)
        {
            out << std::scientific<<std::setprecision(2);
            writeMatrix(mat,width);
        }
    else
        {
            out << std::fixed<<std::setprecision(5);
            writeMatrix(mat,width);
        }
 }


