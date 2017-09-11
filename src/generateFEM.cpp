#include "inputFileReader.h"

void inputFileReader::generateFEModel()
{
    double length = (NUM_BEAMS-1)*BEAM_CC_SPACING + BEAM_OVR_LENGTH*2;
    node n;
    element e;
    int iD = 1;
    // Node Definition

        n.setID(iD);
        n.setProperty(0,0);
        nodeData.push_back(n);
        iD++;
        n.setID(iD);
        n.setProperty(BEAM_OVR_LENGTH*12,0);
        nodeData.push_back(n);
        iD++;

        for (int i=1; i<=NUM_BEAMS-1; i++)
        {
            n.setID(iD);
            iD++;
            n.setProperty(((BEAM_OVR_LENGTH+BEAM_CC_SPACING*i)*12),0);
            nodeData.push_back(n);
        }
        n.setID(iD);
        n.setProperty(length*12,0);
        nodeData.push_back(n);


    // Section Definition
    // Expecting 1 section property
    section s;
    s.setID(1);
    std::vector<double> values;
    values.push_back( Area);
    values.push_back(Inertia);
    s.setProperty(values);
    values.clear();

    // Material Definition
    // Expecting 1 material property
    material m;
    m.setID(1);
    values.push_back(1820*sqrt(PERM_STRESS_C));
    m.setProperty(values);
    values.clear();
    // Element Definition
    node SN, EN;
    std::vector<node> nData;

    for (unsigned int i = 0; i <nodeData.size()-1; i++)
    {
        SN = nodeData[i];
        EN = nodeData[i+1];
        nData.push_back(SN);
        nData.push_back(EN);
        e.setID(i+1);
        e.setProperty(nData,m,s);
        elementData.push_back(e);
        nData.clear();
    }


    // Boundary Definition (GIRDERS SUPPORTED)
    bConditions bCond;
    std::vector<int> bcVal = {0, 1, 0, 0, 0, 0};
    if (BEAM_OVR_LENGTH>0)
        iD = 2;
    else
        iD = 1;

    for (int i=1; i<=NUM_BEAMS; i++)
    {
        if (i==1)
            bcVal[0] = 1;
        else
            bcVal[0] = 0;

        bCond.setProperty(iD,bcVal);
        bcData.push_back(bCond);
        iD++;
    }
    // Load Definition (3 DEFINED LOAD CASES)
    std::vector <int> el;
    values.clear();
    double intensity,dist;
    int lcID = 1;
    iD = 1;
    for (int i=0; i<3; i++)
    {
        switch (i)
        {
        case 0:
        {
            std::vector<load> _lData;
            // Self weight of the slab
            for (unsigned int e=0; e<elementData.size(); e++)
                el.push_back(elementData[e].getID());
            intensity = (WT_SW/(12*12*12*1000))*12*SLAB_THICKNESS*-1;
            load loading1(iD,loadType::EU_Fy,el);
            values.push_back(intensity);
            loading1.setProperty(values);
            loadData.push_back(loading1);
            iD++;
            _lData.push_back(loading1);
            el.clear();
            values.clear();

            // Stay in place deck
            for (unsigned int e=1; e<elementData.size()-1; e++)
                 el.push_back(elementData[e].getID());
            intensity = (WT_SIP/12)*-1;
            load loading2(iD,loadType::EU_Fy,el);
            values.push_back(intensity);
            loading2.setProperty(values);
            loadData.push_back(loading2);
            iD++;
            _lData.push_back(loading2);
            el.clear();
            values.clear();

         // Load combination - 1
            lCombination lc(lcID);
            lc.setProperty(_lData);
            lcData.push_back(lc);
            lcID++;
            _lData.clear();
            break;
        }
        case 1:
        {
             std::vector<load> _lData;

            // Parapet dead load
            // Left End
            int e = elementData[0].getID();
            load loading1(iD,loadType::EC_Fy,e);
            intensity = WT_PARAPET*-1;
            dist = DIST_PARAPET_LEFT * 12;
            values.push_back(intensity);
            values.push_back(dist);
            loading1.setProperty(values);
            values.clear();
            _lData.push_back(loading1);
            iD++;

            // Right End
            e = elementData[elementData.size()-1].getID();
            load loading2(iD,loadType::EC_Fy,e);
            intensity = WT_PARAPET*-1;
            dist = DIST_PARAPET_RIGHT * 12;
            values.push_back(intensity);
            values.push_back(dist);
            loading2.setProperty(values);
            _lData.push_back(loading2);
            iD++;

            // Load combination - 2
            lCombination lc(lcID);
            lc.setProperty(_lData);
            lcData.push_back(lc);
            lcID++;
            _lData.clear();
            values.clear();
            break;
        }
        case 2:
        {
            std::vector<load> _lData;

            // Future wearing surface
            for (unsigned int e=1; e<elementData.size()-1; e++)
                 el.push_back(elementData[e].getID());
            intensity = (WT_FWS/12) *-1;
            load loading1(iD,loadType::EU_Fy,el);
            values.push_back(intensity);
            loading1.setProperty(values);
            loadData.push_back(loading1);
            iD++;
            _lData.push_back(loading1);
            el.clear();
            values.clear();

            int e = 1;
            intensity = (WT_FWS/12) * -1;
            load loading2(iD,loadType::EU_Fy,e);

            int i = e-1;
            int SN = elementData[i].getNodes()[0].getID();
            int EN = elementData[i].getNodes()[1].getID();
            double le = std::sqrt(std::pow(elementData[i].getNodes()[1].getProperty()[0] - elementData[i].getNodes()[0].getProperty()[0],2)
                       + std::pow(elementData[i].getNodes()[1].getProperty()[1] - elementData[i].getNodes()[0].getProperty()[1],2));

            values.push_back(intensity);
            values.push_back(DIST_FWS_LEFT*12);
            values.push_back(le);
            loading2.setProperty(values);
            loadData.push_back(loading2);
            iD++;
            _lData.push_back(loading2);
            el.clear();
            values.clear();

            e = elementData[elementData.size()-1].getID();
            intensity = (WT_FWS/12) * -1;
            load loading3(iD,loadType::EU_Fy,e);
            values.push_back(intensity);
            values.push_back(0.0);
            values.push_back(DIST_FWS_RIGHT*12);
            loading3.setProperty(values);
            loadData.push_back(loading3);
            iD++;
            _lData.push_back(loading3);
            el.clear();
            values.clear();

            // Load combination - 3
            lCombination lc(lcID);
            lc.setProperty(_lData);
            lcData.push_back(lc);
            lcID++;
            _lData.clear();
            values.clear();
            break;
        }
        }
    }

// Display Input Summary
    std::cout << "========================="<< std::endl;
    std::cout << "NODE SUMMARY" << std::endl;
    std::cout << "========================="<< std::endl;
    std::cout <<std::left << std::setw(5) <<"ID" <<std::setw(10) <<"X" << std::setw(10)<< "Y" << std::endl;
    std::cout << "-------------------------"<< std::endl;
    for (unsigned int i=0; i<nodeData.size(); i++)
        nodeData[i].displayProperty();

    std::cout << "========================="<< std::endl;
    std::cout << "ELEMENT SUMMARY" << std::endl;
    std::cout << "========================="<< std::endl;
    std::cout <<std::left << std::setw(5) <<"ID" <<std::setw(10) <<"SN" << std::setw(10)<< "EN" << std::endl;
    std::cout << "-------------------------"<< std::endl;
    for (unsigned int i=0; i<elementData.size(); i++)
        elementData[i].displayProperty();

    std::cout << "========================="<< std::endl;
    std::cout << "BOUNDARY SUMMARY" << std::endl;
    std::cout << "========================="<< std::endl;
    std::cout <<std::left << std::setw(5) <<"ID" <<std::setw(5) <<"dx" << std::setw(5)<< "dy" <<
              std::setw(5)<< "dz" <<std::setw(5)<< "rx" <<std::setw(5)<< "ry" <<std::setw(5)<< "rz" << std::endl;
    std::cout << "-------------------------"<< std::endl;
    for (unsigned int i=0; i<bcData.size(); i++)
        bcData[i].displayProperty();

    std::cout << "========================="<< std::endl;
    std::cout << "LOAD COMBINATION SUMMARY" << std::endl;
    std::cout << "========================="<< std::endl;
    for (unsigned int i=0; i<lcData.size(); i++)
        lcData[i].displayProperty();
}


