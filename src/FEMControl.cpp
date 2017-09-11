#include "inputFileReader.h"

void inputFileReader::startFEM()
{
    FE.start  (nodeData,
               elementData,
               materialData,
               sectionData,
               bcData,
               loadData,
               lcData);

    FE.createIDMatrix();
    FE.createStiffMatrix();
    FE.createForceMatrix();
    FE.applyBC();
    FE.solve();
    FE.populateNodalResData();
    FE.printNodeTable();
    FE.calcElmForces();
    FE.populateElementResData();
    FE.printElementTable();

    //Print Moments
    //Girders (Element 2 to 5)
    std::vector<int> girders = {2,3,4,5};
    std::vector<int> lc = {1,2,3};
    FE.printSectionForces(lc,girders,10,SFType::MOMENT,true,true);

/*
    std::cout << "====================================================================="<<std::endl;
    std::cout << std::left << std::fixed <<std::setw(10) << "Shear" << "Moment" <<std::endl;
    for (double i = 0; i<=1; i=i+0.1)
    {
    int lc = 3;
    int elementID = 6;
    std::cout << std::setprecision(3) << std::setw(10) << std::showpos
    << FE.sectionForce(lc,elementID,i,SFType::SHEAR) << std::setw(10) <<std::showpos
    << FE.sectionForce(lc,elementID,i,SFType::MOMENT)
    << std::endl;
    }
    std::cout << "====================================================================="<<std::endl;

*/
}
