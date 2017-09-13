#include "plate_girder_design.h"

plate_girder_design::  plate_girder_design(std::map<std::string,std::vector<double>> _tokens)
{
    mapTokens(_tokens);
    calcEffwidth();
    calcSectionProperty();
    genModel();
    runFEA();
    report();
}

plate_girder_design::~plate_girder_design()
{

}


void plate_girder_design::mapTokens(std::map<std::string,std::vector<double>> _tokens)
{
    // Check Tokens
    // ---- TO BE IMPLEMENTED ----

    // Map Tokens
    // General Bridge Information
    nSpans = _tokens["$VAR2"][0];
    for (int i=0; i<nSpans; i++ )
        lEff.push_back(_tokens["$VAR1"][i]);
    gSpacing = _tokens["$VAR3"][0];
    nSec   = _tokens["$VAR4"][0];
    for (auto i:_tokens["$VAR5"])
        secLength.push_back(i);
    nGirders = _tokens["$VAR6"][0];
    for (auto i:_tokens["$VAR7"])
        secDef.push_back(i);
    wRoad = _tokens["$VAR8"][0];

    // Section Properties of Girder
    for (int i = 0; i<nGirders; i++)
    {
        tf_width.push_back(_tokens["$VAR11"][i]);
        tf_thk.push_back(_tokens["$VAR12"][i]);
        w_width.push_back(_tokens["$VAR13"][i]);
        w_thk.push_back(_tokens["$VAR14"][i]);
        bf_width.push_back(_tokens["$VAR15"][i]);
        bf_thk.push_back(_tokens["$VAR16"][i]);
    }

    // Slab Properties
    for (int i = 0; i<nGirders; i++)
    {
        slab_thk.push_back(_tokens["$VAR21"][i]);
        haunch_thk.push_back(_tokens["$VAR23"][i]);
    }
    int_ws   = _tokens["$VAR22"][0];
    longRF   = _tokens["$VAR24"][0];

    // Material Properties
    fc      = _tokens["$VAR31"][0];
    if (_tokens["$VAR32"][0] != 0)
        mod_ratio = _tokens["$VAR32"][0];
    else
        mod_ratio = Es/Ec;
    if (_tokens["$VAR33"][0] == 0)
        Ec = 33000*(std::pow(0.150,1.5))* std::sqrt(fc);
    else
        Ec = _tokens["$VAR33"][0];
    Es = 29000;

    if (_tokens["$VAR34"][0] != 0)
        pc = _tokens["$VAR34"][0];
    else
        pc = 0.150/(12*12*12);
    if (_tokens["$VAR35"][0] != 0)
        ps = _tokens["$VAR35"][0];
    else
        ps = 0.490/(12*12*12);

    // Loads
    // Stay-in-place deck form weight
    if (_tokens["$VAR41"][0] != 0)
        wSIP = _tokens["$VAR41"][0];
    else
        wSIP = 0.015/(12*12);
    // Misc. non-composite dead load
    if (_tokens["$VAR42"][0] != 0)
        wMisc = _tokens["$VAR42"][0];
    else
        wMisc = 0.015/12;
    // Parapet load
    if (_tokens["$VAR43"][0] != 0)
        wPar = _tokens["$VAR43"][0];
    else
        wPar = 0.53/12;
    // Future wearing surface load
    if (_tokens["$VAR44"][0] != 0)
        wFWS = _tokens["$VAR44"][0];
    else
        wFWS = 0.140/(12*12*12);
    fws_thk = _tokens["$VAR45"][0];
}

void plate_girder_design::calcEffwidth()
{
    for (int i=0; i<nSec; i++)
    {
        double value = 0;
        // Case - 1
        // 1/4 effective span length
        value = lEff[0]*0.25;
        for (auto i:lEff)
        {
            if (value > i*0.25)
                value = i * 0.25 ;
        }

        // Case - 2
        // 12.0 times average thickness of the slab + Max(web thickness,1/2 x top flange width)
        if (value > (12*(slab_thk[i]-int_ws)+std::max(w_thk[i],0.5*tf_width[i])))
            value = 12*(slab_thk[i]-int_ws)+std::max(w_thk[i],0.5*tf_width[i]);

        // Case - 3
        // average spacing of the adjacent beams
        if (value > gSpacing)
            value = gSpacing;
        bEff.push_back(value);
    }

    for (auto i:bEff)
        i = gSpacing;
    // Print Effective width
/*
    int n = 1;
    for (auto i:bEff)
    {
        std::cout << "Effective width of section - " << n << " = " << i << std::endl;
        n++;
    }
*/

}


void plate_girder_design::calcSectionProperty()
{
    std::vector<shape> values;
    sect secVal;
    shape row;
    double ycg, area, inertia;


    for (int i=0; i<nSec; i++)
    {

        // Steel Girder Property only
        values.clear();
        // Top flange plate
        row.A = tf_width[i]*tf_thk[i];
        row.y = bf_thk[i] + w_width[i] + tf_thk[i]/2;
        row.Ay = row.A * row.y;
        row.Io = tf_width[i]*tf_thk[i]*tf_thk[i]*tf_thk[i]/12;
        values.push_back(row);

        // Web plate
        row.A = w_width[i]*w_thk[i];
        row.y = bf_thk[i] + w_width[i]/2;
        row.Ay = row.A * row.y;
        row.Io = w_thk[i]*w_width[i]*w_width[i]*w_width[i]/12;
        values.push_back(row);

        // Bottom flange plate
        row.A = bf_width[i]*bf_thk[i];
        row.y = bf_thk[i]/2;
        row.Ay = row.A * row.y;
        row.Io = bf_width[i]*bf_thk[i]*bf_thk[i]*bf_thk[i]/12;
        values.push_back(row);

        // CG of the section (A*y/y)
        ycg = (values[0].Ay + values[1].Ay + values[2].Ay)/(values[0].A + values[1].A + values[2].A);
        secVal.yGbot = ycg;
        secVal.yGtop = bf_thk[i] + w_width[i] + tf_thk[i] - ycg;
        secVal.yStop = 0;

        //Total area of the section
        area = values[0].A + values[1].A + values[2].A;
        secVal.A =  area;

        // Distance of CG of the section from CG of each element
        values[0].d = ycg - values[0].y;
        values[1].d = ycg - values[1].y;
        values[2].d = ycg - values[2].y;

        // Moment of inertia about CG of the section
        values[0].I = values[0].Io + values[0].A * values[0].d * values[0].d;
        values[1].I = values[1].Io + values[1].A * values[1].d * values[1].d;
        values[2].I = values[2].Io + values[2].A * values[2].d * values[2].d;
        inertia = values[0].I + values[1].I + values[2].I;
        secVal.I = inertia;
        secVal.data = values;
        secVal.flag = secPropType::GIRDERONLY;
        secProp.push_back(secVal);

        // Composite short term
        values.clear();

        // Top flange plate
        row.A = tf_width[i]*tf_thk[i];
        row.y = bf_thk[i] + w_width[i] + tf_thk[i]/2;
        row.Ay = row.A * row.y;
        row.Io = tf_width[i]*tf_thk[i]*tf_thk[i]*tf_thk[i]/12;
        values.push_back(row);

        // Web plate
        row.A = w_width[i]*w_thk[i];
        row.y = bf_thk[i] + w_width[i]/2;
        row.Ay = row.A * row.y;
        row.Io = w_thk[i]*w_width[i]*w_width[i]*w_width[i]/12;
        values.push_back(row);

        // Bottom flange plate
        row.A = bf_width[i]*bf_thk[i];
        row.y = bf_thk[i]/2;
        row.Ay = row.A * row.y;
        row.Io = bf_width[i]*bf_thk[i]*bf_thk[i]*bf_thk[i]/12;
        values.push_back(row);

        // Slab
        row.A = bEff[i]*(slab_thk[i]-int_ws)/mod_ratio;
        row.y = bf_thk[i] + w_width[i] + haunch_thk[i] + (slab_thk[i]-int_ws)/2;
        row.Ay = row.A * row.y;
        row.Io = bEff[i]/mod_ratio *(slab_thk[i]-int_ws)*(slab_thk[i]-int_ws)*(slab_thk[i]-int_ws)/12;
        values.push_back(row);

        // CG of the section (A*y/y)
        ycg = (values[0].Ay + values[1].Ay + values[2].Ay + values[3].Ay)/(values[0].A + values[1].A + values[2].A + values[3].A);
        secVal.yGbot = ycg;
        secVal.yGtop =  bf_thk[i] + w_width[i] + tf_thk[i] - ycg;
        secVal.yStop =  bf_thk[i] + w_width[i] + haunch_thk[i] + (slab_thk[i]-int_ws) - ycg;

        // Total area of the section
        area = values[0].A + values[1].A + values[2].A + values[3].A;
        secVal.A = area;

        // Distance of CG of the section from CG of each element
        values[0].d = ycg - values[0].y;
        values[1].d = ycg - values[1].y;
        values[2].d = ycg - values[2].y;
        values[3].d = ycg - values[3].y;

        // Moment of inertia about CG of the section
        values[0].I = values[0].Io + values[0].A * values[0].d * values[0].d;
        values[1].I = values[1].Io + values[1].A * values[1].d * values[1].d;
        values[2].I = values[2].Io + values[2].A * values[2].d * values[2].d;
        values[3].I = values[3].Io + values[3].A * values[3].d * values[3].d;
        inertia = values[0].I + values[1].I + values[2].I + values[3].I;
        secVal.I = inertia;
        secVal.data = values;
        secVal.flag = secPropType::COMPOSITE_SHORT;
        secProp.push_back(secVal);

        // Composite long term
        values.clear();

        // Top flange plate
        row.A = tf_width[i]*tf_thk[i];
        row.y = bf_thk[i] + w_width[i] + tf_thk[i]/2;
        row.Ay = row.A * row.y;
        row.Io = tf_width[i]*tf_thk[i]*tf_thk[i]*tf_thk[i]/12;
        values.push_back(row);

        // Web plate
        row.A = w_width[i]*w_thk[i];
        row.y = bf_thk[i] + w_width[i]/2;
        row.Ay = row.A * row.y;
        row.Io = w_thk[i]*w_width[i]*w_width[i]*w_width[i]/12;
        values.push_back(row);

        // Bottom flange plate
        row.A = bf_width[i]*bf_thk[i];
        row.y = bf_thk[i]/2;
        row.Ay = row.A * row.y;
        row.Io = bf_width[i]*bf_thk[i]*bf_thk[i]*bf_thk[i]/12;
        values.push_back(row);

        // Slab
        row.A = bEff[i] *(slab_thk[i]-int_ws)/(3*mod_ratio);
        row.y = bf_thk[i] + w_width[i] + haunch_thk[i] + (slab_thk[i]-int_ws)/2;
        row.Ay = row.A * row.y;
        row.Io = bEff[i]/(3*mod_ratio) *(slab_thk[i]-int_ws)*(slab_thk[i]-int_ws)*(slab_thk[i]-int_ws)/12;
        values.push_back(row);


        // CG of the section (A*y/y)
        ycg = (values[0].Ay + values[1].Ay + values[2].Ay + values[3].Ay)/(values[0].A + values[1].A + values[2].A + values[3].A);
        secVal.yGbot = ycg;
        secVal.yGtop =  bf_thk[i] + w_width[i] + tf_thk[i] - ycg;
        secVal.yStop =  bf_thk[i] + w_width[i] + haunch_thk[i] + (slab_thk[i]-int_ws) - ycg;

        // Total area of the section
        area = values[0].A + values[1].A + values[2].A + values[3].A;
        secVal.A = area;

        // Distance of CG of the section from CG of each element
        values[0].d = ycg - values[0].y;
        values[1].d = ycg - values[1].y;
        values[2].d = ycg - values[2].y;
        values[3].d = ycg - values[3].y;

        // Moment of inertia about CG of the section
        values[0].I = values[0].Io + values[0].A * values[0].d * values[0].d;
        values[1].I = values[1].Io + values[1].A * values[1].d * values[1].d;
        values[2].I = values[2].Io + values[2].A * values[2].d * values[2].d;
        values[3].I = values[3].Io + values[3].A * values[3].d * values[3].d;
        inertia = values[0].I + values[1].I + values[2].I + values[3].I;
        secVal.I = inertia;
        secVal.data = values;
        secVal.flag = secPropType::COMPOSITE_LONG;
        secProp.push_back(secVal);


        // Composite deck RF only
        values.clear();

        // Top flange plate
        row.A = tf_width[i]*tf_thk[i];
        row.y = bf_thk[i] + w_width[i] + tf_thk[i]/2;
        row.Ay = row.A * row.y;
        row.Io = tf_width[i]*tf_thk[i]*tf_thk[i]*tf_thk[i]/12;
        values.push_back(row);

        // Web plate
        row.A = w_width[i]*w_thk[i];
        row.y = bf_thk[i] + w_width[i]/2;
        row.Ay = row.A * row.y;
        row.Io = w_thk[i]*w_width[i]*w_width[i]*w_width[i]/12;
        values.push_back(row);

        // Bottom flange plate
        row.A = bf_width[i]*bf_thk[i];
        row.y = bf_thk[i]/2;
        row.Ay = row.A * row.y;
        row.Io = bf_width[i]*bf_thk[i]*bf_thk[i]*bf_thk[i]/12;
        values.push_back(row);

        // Slab Reinforcement
        row.A = longRF;
        row.y = bf_thk[i] + w_width[i] + haunch_thk[i] + (slab_thk[i]-2.5-1.0)/2 + 1.0;
        row.Ay = row.A * row.y;
        row.Io = 0;
        values.push_back(row);


        // CG of the section (A*y/y)
        ycg = (values[0].Ay + values[1].Ay + values[2].Ay + values[3].Ay)/(values[0].A + values[1].A + values[2].A + values[3].A);
        secVal.yGbot = ycg;
        secVal.yGtop =  bf_thk[i] + w_width[i] + tf_thk[i] - ycg;
        secVal.yStop =  bf_thk[i] + w_width[i] + haunch_thk[i] + 1.0 + (slab_thk[i]-2.5-1.0)/2 - ycg;

        // Total area of the section
        area = values[0].A + values[1].A + values[2].A + values[3].A;
        secVal.A = area;

        // Distance of CG of the section from CG of each element
        values[0].d = ycg - values[0].y;
        values[1].d = ycg - values[1].y;
        values[2].d = ycg - values[2].y;
        values[3].d = ycg - values[3].y;

        // Moment of inertia about CG of the section
        values[0].I = values[0].Io + values[0].A * values[0].d * values[0].d;
        values[1].I = values[1].Io + values[1].A * values[1].d * values[1].d;
        values[2].I = values[2].Io + values[2].A * values[2].d * values[2].d;
        values[3].I = values[3].Io + values[3].A * values[3].d * values[3].d;
        inertia = values[0].I + values[1].I + values[2].I + values[3].I;
        secVal.I = inertia;
        secVal.data = values;
        secVal.flag = secPropType::COMPOSITE_RF;
        secProp.push_back(secVal);
        values.clear();


    }


    std::cout << "Section Properties Calculated" << std::endl;


// To Print
    for (auto k:secProp)
    {
        for (auto i:k.data)
        {
            std::cout << std::setw(10) << i.A
                      << std::setw(10) << i.y
                      << std::setw(10) << i.Ay
                      << std::setw(10) << i.Io
                      << std::setw(10) << i.d
                      << std::setw(10) << i.I
                      << std::endl;

        }
        std::cout << "==========================================================="<< std::endl;
        std::cout << "Total Area = " << k.A <<std::endl;
        std::cout << "Total Inertia = " << k.I <<std::endl;
        std::cout << "Distance of CG to bottom of girder = " << k.yGbot <<std::endl;
        std::cout << "Distance of CG to top of girder = " << k.yGtop <<std::endl;
        std::cout << "Distance of CG to top of slab = " << k.yStop <<std::endl;

    }
}


void plate_girder_design::genModel()
{


    // Create Nodes
    node n;
    int id = 1;
    double length = 0;
    n.setID(id);
    id++;
    n.setProperty(0,0);
    nodeData.push_back(n);
    for (auto j:secLength)
    {
        n.setID(id);
        id++;
        length = length + j;
        n.setProperty(length,0);
        nodeData.push_back(n);
    }

    // To Print Nodes
    std::cout << "Nodal Data" <<std::endl;
    for (auto i : nodeData)
        std::cout << i.getID() << "\t" << i.getProperty()[0] << "\t" << i.getProperty()[1] << std::endl;


    // Create Section Property
    std::vector<double> values;
    section s;
    std::vector<section> sData;
    std::vector<double> data;
    id=1;
    int k=0;
    for (unsigned int i=0; i<secProp.size(); i++)
    {
        values.clear();
        values.push_back(tf_width[secDef[k]-1]);
        values.push_back(tf_thk[secDef[k]-1]);

        s.setTF(values);
        s.setID(id); id++;
        data.push_back(secProp[i].A);
        data.push_back(secProp[i].I);
        s.setProperty(data);
        sData.push_back(s);
        data.clear();
        if (sData.size()==4)
        {
            k++;
            sectionData.push_back(sData);
            sData.clear();
        }
    }

    // To Print Section Properties
    std::cout << "Section Data" <<std::endl;
    id = 1;
    for (auto i : sectionData)
    {
        std::cout << "Section - " << id <<std::endl; id++;
        std::cout << "Non-Composite : "
                  << i.getProperty(secPropType::GIRDERONLY).getProperty()[0]
                  << "\t"
                  << i.getProperty(secPropType::GIRDERONLY).getProperty()[1]
                  << std::endl;
        std::cout << "Composite Short Term : "
                  << i.getProperty(secPropType::COMPOSITE_SHORT).getProperty()[0]
                  << "\t"
                  << i.getProperty(secPropType::COMPOSITE_SHORT).getProperty()[1]
                  << std::endl;
        std::cout << "Composite Long Term : "
                  << i.getProperty(secPropType::COMPOSITE_LONG).getProperty()[0]
                  << "\t"
                  << i.getProperty(secPropType::COMPOSITE_LONG).getProperty()[1]
                  << std::endl;
        std::cout << "Composite Reinforcement : "
                  << i.getProperty(secPropType::COMPOSITE_RF).getProperty()[0]
                  << "\t"
                  << i.getProperty(secPropType::COMPOSITE_RF).getProperty()[1]
                  << std::endl;

    }

    // Create Material Property (Steel Only)
    material mate;
    mate.setID(1);
    data.clear();
    data.push_back(Es);
    mate.setProperty(data);
    data.clear();
    materialData.push_back(mate);

    // Create Elements
    element e;
    std::vector<node> nData;

    id = 1;
    for (unsigned int i=0; i<secDef.size(); i++)
    {
        e.setID(id); id++;
        nData.push_back(nodeData[i]);
        nData.push_back(nodeData[i+1]);
        // Non-Composite Section
        e.setProperty(nData,materialData[0],sectionData[secDef[i]-1].getProperty(secPropType::GIRDERONLY));
        elementDataNC.push_back(e);
        // Composite Section (Short)
        e.setProperty(nData,materialData[0],sectionData[secDef[i]-1].getProperty(secPropType::COMPOSITE_SHORT));
        elementDataCS.push_back(e);
        // Composite Section (Long)
        e.setProperty(nData,materialData[0],sectionData[secDef[i]-1].getProperty(secPropType::COMPOSITE_LONG));
        elementDataCL.push_back(e);
        // Composite Section (Reinf.)
        e.setProperty(nData,materialData[0],sectionData[secDef[i]-1].getProperty(secPropType::COMPOSITE_RF));
        elementDataCR.push_back(e);
        nData.clear();
    }

    // Print Element Data
    // Non-composite Elements
    std::cout << "Non-Composite element data" << std::endl;
    std::cout << "--------------------------" << std::endl;
    std::cout << std::setw(10) << "Element"
              << std::setw(10) << "SN"
              << std::setw(10) << "EN"
              << std::setw(10) << "EMod"
              << std::setw(10) << "Area"
              << std::setw(10) << "Inertia"
              << std::endl;
    for (auto i:elementDataNC)
    {
        std::cout << std::setw(10) << i.getID()
              << std::setw(10) << i.getNodes()[0].getID()
              << std::setw(10) << i.getNodes()[1].getID()
              << std::setw(10) << i.getMaterial().getProperty()[0]
              << std::setw(10) << i.getSection().getProperty()[0]
              << std::setw(10) << i.getSection().getProperty()[1]
              << std::endl;
    }

 // Composite Elements (Short)
    std::cout << "Composite (Short) element data" << std::endl;
    std::cout << "-------------------------------" << std::endl;
    std::cout << std::setw(10) << "Element"
              << std::setw(10) << "SN"
              << std::setw(10) << "EN"
              << std::setw(10) << "EMod"
              << std::setw(10) << "Area"
              << std::setw(10) << "Inertia"
              << std::endl;
    for (auto i:elementDataCS)
    {
        std::cout << std::setw(10) << i.getID()
              << std::setw(10) << i.getNodes()[0].getID()
              << std::setw(10) << i.getNodes()[1].getID()
              << std::setw(10) << i.getMaterial().getProperty()[0]
              << std::setw(10) << i.getSection().getProperty()[0]
              << std::setw(10) << i.getSection().getProperty()[1]
              << std::endl;
    }

     // Composite Elements (Long)
    std::cout << "Composite (Long) element data" << std::endl;
    std::cout << "------------------------------" << std::endl;
    std::cout << std::setw(10) << "Element"
              << std::setw(10) << "SN"
              << std::setw(10) << "EN"
              << std::setw(10) << "EMod"
              << std::setw(10) << "Area"
              << std::setw(10) << "Inertia"
              << std::endl;
    for (auto i:elementDataCL)
    {
        std::cout << std::setw(10) << i.getID()
              << std::setw(10) << i.getNodes()[0].getID()
              << std::setw(10) << i.getNodes()[1].getID()
              << std::setw(10) << i.getMaterial().getProperty()[0]
              << std::setw(10) << i.getSection().getProperty()[0]
              << std::setw(10) << i.getSection().getProperty()[1]
              << std::endl;
    }

 // Composite (RF) Elements
    std::cout << "Composite (RF) element data" << std::endl;
    std::cout << "--------------------------" << std::endl;
    std::cout << std::setw(10) << "Element"
              << std::setw(10) << "SN"
              << std::setw(10) << "EN"
              << std::setw(10) << "EMod"
              << std::setw(10) << "Area"
              << std::setw(10) << "Inertia"
              << std::endl;
    for (auto i:elementDataCR)
    {
        std::cout << std::setw(10) << i.getID()
              << std::setw(10) << i.getNodes()[0].getID()
              << std::setw(10) << i.getNodes()[1].getID()
              << std::setw(10) << i.getMaterial().getProperty()[0]
              << std::setw(10) << i.getSection().getProperty()[0]
              << std::setw(10) << i.getSection().getProperty()[1]
              << std::endl;
    }




    // Create Boundary Conditions
    bConditions bc;
    std::vector<int> bcVal = {1, 1, 0, 0, 0, 0};
    //first boundary node
    bc.setProperty(1,bcVal);
    bcData.push_back(bc);
    //other boundary nodes
    bcVal[0] = 0;
    length = 0;
    for (auto i:lEff)
    {
        length += i;
        bc.setProperty(findNode(length),bcVal);
        bcData.push_back(bc);
    }

    //Print Boundary Conditions
    std::cout << "Boundary Data" << std::endl;
    for (auto i:bcData)
    {
        std::cout << std::setw(10) << i.getID() << std::setw(10)
                  << i.getProperty()[0] << std::setw(10)
                  << i.getProperty()[1] << std::setw(10)
                  << i.getProperty()[2] << std::setw(10)
                  << i.getProperty()[3] << std::setw(10)
                  << i.getProperty()[4] << std::setw(10)
                  << i.getProperty()[5] << std::setw(10)
                  << std::endl;
    }

    // Create Loads

    std::vector<load> loadData;
    std::vector <int> el;
    values.clear();
    double intensity,dist;
    int lcID = 1;
    int iD = 1;

    for (int i=1; i<=5; i++)
    {
        switch (i)
        {
            case 1:  // Dead Load of steel girder
            {
                for (unsigned int e=0; e<elementDataNC.size(); e++)
                {
                    el.push_back(elementDataNC[e].getID());
                    intensity = ps * elementDataNC[e].getSection().getProperty()[0] * -1;
                    load loading(iD,loadType::EU_Fy,el);
                    values.push_back(intensity);
                    loading.setProperty(values);
                    loadData.push_back(loading);
                    iD++;
                    el.clear();
                    values.clear();

                }
                // Load combination - 1
                lCombination lc(lcID);
                lc.setProperty(loadData);
                lcData.push_back(lc);
                lcID++;
                loadData.clear();
                break;

            }

            case 2: // Concrete deck
            {
               for (unsigned int e=0; e<elementDataNC.size(); e++)
                {
                    std::cout << "concrete unit weight = " << pc << std::endl;
                    std::cout << "width = " << elementDataNC[e].getSection().getTF()[0] <<std::endl;
                    std::cout << "depth = " << elementDataNC[e].getSection().getTF()[1] <<std::endl;
                    std::cout << "gspacing = " << gSpacing << std::endl;
                    el.push_back(elementDataNC[e].getID());
                    intensity = pc * (8.5 * gSpacing  + (haunch_thk[0] - elementDataNC[e].getSection().getTF()[1]) *  (elementDataNC[e].getSection().getTF()[0]))  * -1;
                    load loading(iD,loadType::EU_Fy,el);
                    values.push_back(intensity);
                    loading.setProperty(values);
                    loadData.push_back(loading);
                    iD++;
                    el.clear();
                    values.clear();

                }
                // Load combination - 2
                lCombination lc(lcID);
                lc.setProperty(loadData);
                lcData.push_back(lc);
                lcID++;
                loadData.clear();
                break;
            }

            case 3: // Misc Load
            {
               for (unsigned int e=0; e<elementDataNC.size(); e++)
                {
                    el.push_back(elementDataNC[e].getID());
                    intensity = (wSIP*gSpacing+wMisc);
                    load loading(iD,loadType::EU_Fy,el);
                    values.push_back(intensity);
                    loading.setProperty(values);
                    loadData.push_back(loading);
                    iD++;
                    el.clear();
                    values.clear();

                }
                // Load combination - 3
                lCombination lc(lcID);
                lc.setProperty(loadData);
                lcData.push_back(lc);
                lcID++;
                loadData.clear();
                break;
            }

            case 4: // Parapet Load
            {
               for (unsigned int e=0; e<elementDataCL.size(); e++)
                {
                    el.push_back(elementDataCL[e].getID());
                    intensity = wPar*2/nGirders;
                    load loading(iD,loadType::EU_Fy,el);
                    values.push_back(intensity);
                    loading.setProperty(values);
                    loadData.push_back(loading);
                    iD++;
                    el.clear();
                    values.clear();

                }
                // Load combination - 4
                lCombination lc(lcID);
                lc.setProperty(loadData);
                lcData.push_back(lc);
                lcID++;
                loadData.clear();
                break;
            }

            case 5: // Future Wearing Surface
            {
               for (unsigned int e=0; e<elementDataCL.size(); e++)
                {
                    el.push_back(elementDataCL[e].getID());
                    intensity = wFWS*fws_thk*wRoad/nGirders;
                    load loading(iD,loadType::EU_Fy,el);
                    values.push_back(intensity);
                    loading.setProperty(values);
                    loadData.push_back(loading);
                    iD++;
                    el.clear();
                    values.clear();
                }
                // Load combination - 5
                lCombination lc(lcID);
                lc.setProperty(loadData);
                lcData.push_back(lc);
                lcID++;
                loadData.clear();
                break;
            }


        }
}
}

int plate_girder_design::findSection(double distance)
{
    double length = 0;
    for (auto i:lEff)
        length += i;

    if (distance > length )
        std::cout << "Requested distance is beyond the limit of the span" << std::endl;
    length = 0;
   int id=0;
    for (auto i:secLength)
    {
        if (distance >= length && distance <= length+i)
            return id;
        else
            id++;
    }
    std::cout << "Requested distance is not found in any section" << std::endl;
    return 0;
}

int plate_girder_design::findNode(double distance)
{
    for (auto i:nodeData)
    {
        if (i.getProperty()[0] == distance)
        {
            return i.getID();
        }
    }
    std::cout << "Could not find node at requested location" << std::endl;
    return 1;
}



void plate_girder_design::runFEA()
{
    FE.start  (nodeData,
               elementDataCL,
               bcData,
               lcData);

    FE.run();
    FE.print();

    int lc = 4;
    int elementID = 4;
    double distance = 0.0;

    std::cout << "====================================================================="<<std::endl;
    std::cout << "Axial force = " << FE.sectionForce(lc,elementID,distance,SFType::AXIAL) << std::endl;
    std::cout << "====================================================================="<<std::endl;

    std::cout << "====================================================================="<<std::endl;
    std::cout << "Shear force = " << FE.sectionForce(lc,elementID,distance,SFType::SHEAR) << std::endl;
    std::cout << "====================================================================="<<std::endl;

    std::cout << "====================================================================="<<std::endl;
    std::cout << "Bending Moment = " << FE.sectionForce(lc,elementID,distance,SFType::MOMENT)/12 << std::endl;
    std::cout << "====================================================================="<<std::endl;


}
