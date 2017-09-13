#include "FEMEngine.h"

void FEMEngine::printNodeTable()
{
    double tol = 1e-6;
   // Write output to file
    writer output("nodeTable.txt");

// Write Input Table
    // Table Template
for (unsigned int i=0; i< loadComboData.size(); i++)
{
    rowData elm;   // row data structure
    int rows = int(nodalData.size()); // number of rows
    int cols = 7; // number of columns
    std::vector<rowData> dataTable; //vector of row data (DATA)
    std::vector<rowData> reportTable; // vector of row data (REPORT)
    std::vector<std::string> headers; //vector of header strings
    std::vector<int> colSize;   //vector of row width
    std::vector<int> align;     //vector for text alignment
    table tb_HR;  //table object

    // For constant width of columns by default
    int cons_width = 15;
    for (int i = 0; i<cols; i++)
        colSize.push_back(cons_width);

    // For Text Alignment
    int cons_align = 1; // 0 - left, 1 - right
    for (int i = 0; i<cols; i++)
        align.push_back(cons_align);

    // Table Headers
    headers.push_back("Node");
    headers.push_back("dX");
    headers.push_back("dY");
    headers.push_back("rZ");
    headers.push_back("Rx");
    headers.push_back("Ry");
    headers.push_back("Rz");

    for (int j=0; j<rows; j++)
    {
        elm.push(toString(j+1));
        elm.push(toString(nodalRData[i][j].getDsp()[0],tol));
        elm.push(toString(nodalRData[i][j].getDsp()[1],tol));
        elm.push(toString(nodalRData[i][j].getDsp()[2],tol));
        elm.push(toString(nodalRData[i][j].getReac()[0],tol));
        elm.push(toString(nodalRData[i][j].getReac()[1],tol));
        elm.push(toString(nodalRData[i][j].getReac()[2],tol));

        dataTable.push_back(elm);
        elm.clear();
    }

    // Set Table Data
    tb_HR.setTable(headers,dataTable,colSize,align);


    output.writeTable(tb_HR,"Displacement Table (Load Case: "+toString(i+1)+")");
    //output.writeTableCopy(tb_HR,"Displacement Table");
}
    output.close();



}

void FEMEngine::printElementTable()
{
double tol = 1e-6;
// Write output to file
writer output("elementTable.txt");

// Write Input Table
// Table Template
for (unsigned int i=0; i< loadComboData.size(); i++)
{
    rowData elm;   // row data structure
    int cols = 5; // number of columns
    std::vector<rowData> dataTable; //vector of row data (DATA)
    std::vector<rowData> reportTable; // vector of row data (REPORT)
    std::vector<std::string> headers; //vector of header strings
    std::vector<int> colSize;   //vector of row width
    std::vector<int> align;     //vector for text alignment
    table tb_HR;  //table object

    // For constant width of columns by default
    int cons_width = 15;
    for (int i = 0; i<cols; i++)
        colSize.push_back(cons_width);

    // For Text Alignment
    int cons_align = 1; // 0 - left, 1 - right
    for (int i = 0; i<cols; i++)
        align.push_back(cons_align);

    // Table Headers
    headers.push_back("Element");
    headers.push_back("Node");
    headers.push_back("FX");
    headers.push_back("FY");
    headers.push_back("MZ");

    for (unsigned int j=0; j<elementData.size(); j++)
    {
        for (int k=0; k<2; k++)
        {
            if(k==0)
            {
                elm.push(toString(elementData[j].getID()));
                elm.push(toString(elementRData[i][j].getSNode()));
                elm.push(toString(elementRData[i][j].getProperty()[0],tol));
                elm.push(toString(elementRData[i][j].getProperty()[1],tol));
                elm.push(toString(elementRData[i][j].getProperty()[2],tol));

            }
            else
            {
                elm.push(toString(" "));
                elm.push(toString(elementRData[i][j].getENode()));
                elm.push(toString(elementRData[i][j].getProperty()[3],tol));
                elm.push(toString(elementRData[i][j].getProperty()[4],tol));
                elm.push(toString(elementRData[i][j].getProperty()[5],tol));
            }
            dataTable.push_back(elm);
            elm.clear();
        }



    }

    // Set Table Data
    tb_HR.setTable(headers,dataTable,colSize,align);


    output.writeTable(tb_HR,"Element Forces Table (Load Case: "+toString(i+1)+")");
    //output.writeTableCopy(tb_HR,"Displacement Table");
}
    output.close();



}


  void FEMEngine::printSectionForces(std::vector<int> _lc, std::vector<int> _elmID, int _div, SFType _type,
                                     bool _revSign, bool _ft)
  {
    double tol = 1e-6;
    // Write output to file
    writer output("sectionForces.txt");
    for (unsigned int lc = 0; lc < _lc.size(); lc++)
    {


    rowData elm;   // row data structure
    int cols = 2 + _div; // number of columns
    std::vector<rowData> dataTable; //vector of row data (DATA)
    std::vector<rowData> reportTable; // vector of row data (REPORT)
    std::vector<std::string> headers; //vector of header strings
    std::vector<int> colSize;   //vector of row width
    std::vector<int> align;     //vector for text alignment
    table tb_HR;  //table object

    // For constant width of columns by default
    int cons_width = 5;
    for (int i = 0; i<cols; i++)
        colSize.push_back(cons_width);
    colSize[0] = 10;

    // For Text Alignment
    int cons_align = 1; // 0 - left, 1 - right
    for (int i = 0; i<cols; i++)
        align.push_back(cons_align);
    align[0] = 0;
    // Table Headers
    headers.push_back("Distance");
    for (double i = 0.0; i<=1.0; i=i + 0.1)
        headers.push_back(toString(i,tol,false));

    for (unsigned int i=0; i<_elmID.size(); i++)
    {
                elm.push("Bay " + toString(elementData[_elmID[i] - 1].getID()));
                for (double k = 0; k <= 1.0; k=k+0.1)
                {
                    //Change here for kip-ft in Moment by dividing by 12
                    if (_ft == true && _type==SFType::MOMENT)
                    {
                        if(_revSign == true)
                            elm.push(toString(sectionForce(_lc[lc],_elmID[i],k,_type)/12*-1,tol,false));
                        else
                            elm.push(toString(sectionForce(_lc[lc],_elmID[i],k,_type)/12,tol,false));
                    }
                    else
                    {
                        if (_revSign == true)
                            elm.push(toString(sectionForce(_lc[lc],_elmID[i],k,_type)*-1,tol,false));
                        else
                            elm.push(toString(sectionForce(_lc[lc],_elmID[i],k,_type),tol,false));

                    }
                        }

            dataTable.push_back(elm);
            elm.clear();
    }



    // Set Table Data
    tb_HR.setTable(headers,dataTable,colSize,align);

    output.writeTable(tb_HR,"Element Forces Table (Load Case: "+toString(_lc[lc])+")");
    }
    output.close();


  }
