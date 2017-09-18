#include "infLine.h"

infLine::infLine(std::vector<node> _nodeData,
                   std::vector<element> _elementData,
                   std::vector<bConditions> _bcData,
                   std::vector<int> _elementILD)
{
    nodeData = _nodeData;
    elementData = _elementData;
    bcData = _bcData;

    elementILD = _elementILD;
    parts = 20;

}

void infLine::run()
{
    // Unit load
    double intensity = 1;
    // Distance
    double distance;
    // Element ID
    int eID;
    // Load ID
    int loadID = 1;
    // Load combination ID
    int lcID = 1;
    // Load vector
    std::vector<load> loadData;

    std::vector<double> values;
    double le;

    for (auto e:elementData)
    {
        eID = e.getID();
        le = std::sqrt(std::pow(e.getNodes()[1].getProperty()[0] - e.getNodes()[0].getProperty()[0],2)
                       + std::pow(e.getNodes()[1].getProperty()[1] - e.getNodes()[0].getProperty()[1],2));

        for (int p = 0; p <= parts; p++)
        {
            values.clear();
            distance = p*le/parts;
            values.push_back(intensity);
            values.push_back(distance);
            load loading(loadID,loadType::EC_Fy,eID);
            loading.setProperty(values);
            loadData.push_back(loading);
            loadID++;
            values.clear();

           // Load combination
            lCombination lc(lcID);
            lc.setProperty(loadData);
            lcData.push_back(lc);
            lcID++;
            loadData.clear();
        }
    }


    FE.start   (nodeData,
               elementData,
               bcData,
               lcData);
    FE.run();

    infV.setSize(lcData.size(),lcData.size());
    infM.setSize(lcData.size(),lcData.size());

    for (unsigned int i=0; i<lcData.size();i++)
    {
        for (unsigned int j=0; j<elementData.size(); j++)
        {
            for(double k=0; k<=parts; k++)
            {
                infV(i+1,j*parts+k+1) = FE.sectionForce(lcData[i].getID(),elementData[j].getID(),k/parts , SFType::SHEAR);
                infM(i+1,j*parts+k+1) = FE.sectionForce(lcData[i].getID(),elementData[j].getID(),k/parts , SFType::MOMENT);
            }
        }
    }
    std::vector<double> gDist;
    distance = 0;
    for (unsigned int e=0; e<elementData.size();e++)
    {
        for (double p=0; p<=parts; p++)
        {
            le  = std::sqrt(std::pow(elementData[e].getNodes()[1].getProperty()[0] - elementData[e].getNodes()[0].getProperty()[0],2)
                       + std::pow(elementData[e].getNodes()[1].getProperty()[1] - elementData[e].getNodes()[0].getProperty()[1],2));
            distance  = elementData[e].getNodes()[0].getProperty()[0] + le/parts * p;
            gDist.push_back(distance);

            }
    }

    // Normalize the matrix by column vectors
    mathlib matToolBox;
    matToolBox.normByColumns(infV);
    matToolBox.normByColumns(infM);

    writer outILD("output_ILD.txt");
    int lc = 100;
    std::cout << "Distance = " << lcData[lc-1].getProperty()[0].getElements()[0] << "  " <<
    lcData[lc-1].getProperty()[0].getValues()[0]<< "  "<<
    lcData[lc-1].getProperty()[0].getValues()[1]<< std::endl;
    for (unsigned int i=1; i<=infV.rowSize();i++)
    {
        outILD.writeText (toString(gDist[i-1]) + "\t" + toString(infV(i,lc)));

    }
    outILD.close();

    writer outMat("output_matrix.txt");
    outMat.writeMatrix(infV,10);
    outMat.close();

}







infLine::~infLine()
{

}
