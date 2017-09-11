#pragma once
#ifndef INPUTFILEREADER_H
#define INPUTFILEREADER_H

#include "reader.h"
#include <string>
#include "node.h"
#include "element.h"
#include "material.h"
#include "section.h"
#include "bConditions.h"
#include "load.h"
#include "lcombination.h"
#include <vector>
#include "writer.h"
#include <iomanip>
#include "FEMEngine.h"

enum class token
{
    BEAM_CC_SPACING,
    BEAM_OVR_LENGTH,
    NUM_BEAMS,
    COVER_TOP,
    COVER_BOTTOM,
    PERM_STRESS_C,
    PERM_STRESS_S,
    WT_SW,
    WT_FWS,
    WT_PARAPET,
    WT_SIP,
    SLAB_THICKNESS,
    SECTION,
    NULL_TOKEN
};

class inputFileReader
{
    public:
        inputFileReader(std::string _fileName);
        ~inputFileReader();

        void read();
        token tokenCheck(std::vector<std::string> line);
        void tokenAction(token tok, std::vector<std::string> &line);
        void generateFEModel();
        void startFEM     ();

    protected:

    private:
        std::string fileName;
        double BEAM_CC_SPACING  = 0;
        double BEAM_OVR_LENGTH = 0;
        double NUM_BEAMS = 0;
        double COVER_TOP = 2.5;
        double COVER_BOTTOM = 1;
        double PERM_STRESS_C = 4.0;
        double PERM_STRESS_S = 60;
        double WT_SW = 150;
        double WT_FWS = 0.024;
        double WT_PARAPET = 0.39;
        double WT_SIP = 0.015;
        double SLAB_THICKNESS = 8.0;
        double Area = 96;
        double Inertia = 512;
        double DIST_PARAPET_LEFT = 0; //0.6875;
        double DIST_PARAPET_RIGHT = 0; //2.3958;
        double DIST_FWS_LEFT = 0; //1.5808;
        double DIST_FWS_RIGHT = 0; //1.5;

        std::vector<node> nodeData;
        std::vector<element> elementData;
        std::vector<material> materialData; // Unused - Only 1 material
        std::vector<section> sectionData;   // Unused - Only 1 section
        std::vector<bConditions> bcData;
        std::vector<lCombination> lcData;   // Predefined 3 cases
        std::vector<load> loadData;
        FEMEngine FE;
};

#endif // INPUTFILEREADER_H
