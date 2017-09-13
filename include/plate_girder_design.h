#ifndef PLATE_GIRDER_DESIGN_H
#define PLATE_GIRDER_DESIGN_H

#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <fstream>
#include "FEMEngine.h"



struct shape
{
    double A,y,Ay,Io,d,I;
};

struct sect
{
    secPropType flag;
    std::vector<shape> data;
    double A,I;
    double yGtop, yGbot, yStop;
    double zGtop, zGbot, zStop;

};

class plate_girder_design
{
    public:
        plate_girder_design(std::map<std::string,std::vector<double>> _tokens);
        ~plate_girder_design();


    protected:

    private:

        //Map Tokens
        void mapTokens(std::map<std::string,std::vector<double>> _tokens);

        //Calculate effective width of deck slab
        void calcEffwidth();

        //Calculate section properties
        void calcSectionProperty();

        //General Bridge Information
        double nSpans, gSpacing, nSec, nGirders,wRoad;
        std::vector<double> lEff, bEff, secLength, secDef;

        //Section Properties
        std::vector<double> tf_width, tf_thk, w_width, w_thk, bf_width, bf_thk;
        std::vector<sect> secProp;

        //Slab Properties
        double int_ws, longRF;
        std::vector<double> slab_thk, haunch_thk;

        //Material Properties
        double mod_ratio, Ec, fc, Es,pc,ps;

        //Load Parameters
        double wSIP, wMisc, wPar, wFWS ;
        double fws_thk;

        //FE Model Components

        void genModel();

        int findSection(double distance);
        int findNode(double distance);

        std::vector<node> nodeData;
        std::vector<element> elementDataNC, elementDataCS, elementDataCL, elementDataCR ;
        std::vector<material> materialData;
        std::vector<sectionGroup> sectionData;


        std::vector<bConditions> bcData;
        std::vector<lCombination> lcData;   // Predefined 3 cases

        FEMEngine FE;
        //Start FEA
        void runFEA();

        //Report Generation
        std::ofstream out;
        void report();
        void createPDF();
        void repSecProp();

};

#endif // PLATE_GIRDER_DESIGN_H
