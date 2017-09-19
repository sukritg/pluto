#pragma once
#include <vector>
#include <cmath>
enum class sectionType
{
    PLATEGIRDER,
    GENERAL

};
enum class stage
{
    NONCOMPOSITE,
    COMPOSITE_LONG,
    COMPOSITE_SHORT,
    COMPOSITE_REINFORCEMENT

};


struct sectionTable
{
    double w_tf,w_web,w_bf,w_slab;
    double d_tf,d_web,d_bf,d_slab;
    double A_tf,A_web,A_bf,A_slab,A_rf;
    double y_tf,y_web,y_bf,y_slab,y_rf;
    double Ay_tf,Ay_web,Ay_bf,Ay_slab,Ay_rf;
    double Ay2_tf,Ay2_web,Ay2_bf,Ay2_slab,Ay2_rf;
    double Io_tf,Io_web,Io_bf,Io_slab,Io_rf;
    double I_tf,I_web,I_bf,I_slab,I_rf;
    double m;
    double Ay2,Ay,A,y,Io,I;

};

class section
{
    private:

    //Section Type
    sectionType xType;

    //Section ID
    int sectionID;

    // General Section
    double A,I,y;

    // Plate Girder Section
    double w_tf,w_web,w_bf,w_slab;
    double d_tf,d_web,d_bf,d_slab;
    double As, haunch,m;


    std::vector<double> tf;

    public:

    section()
    {
        xType = sectionType::GENERAL;
        A = 0; I = 0; y =0;
    }

    void setType(sectionType _xType)
    {
        xType = _xType;
    }

    sectionType getType()
    {
        return xType;
    }

    void setProperty(std::vector<double> &data)
    {
        switch (xType)
        {
        case sectionType::GENERAL :
            {
                A = data[0];
                I = data[1];
                break;
            }
        case sectionType::PLATEGIRDER :
            {
            //Dimensions
            w_tf = data[0];
            w_web = data[1];
            w_bf = data[2];
            w_slab = data[3];
            d_tf = data[4];
            d_web = data[5];
            d_bf = data[6];
            d_slab = data[7];
            As = data[8];
            haunch = data[9];

            //Modular ratio
            if (data.size()==11)
                m = data[10];
            else
                m = 8;
                break;
            }
        }
    }

    std::vector<double> getProperty()
    {
        std::vector<double> data;
        data.push_back(A);
        data.push_back(I);
        return data;
    }

    int getID()
    {
        return sectionID;
    }

    void setID(int id)
    {
        sectionID = id;
    }



    //optional: set top flange width of element
    void setTF(std::vector<double> _values)
    {
        tf.clear();
        tf = _values;

    }

    //optional: get top flange width of element
    std::vector<double> getTF()
    {
        return tf;
    }

    //Calculate section properties
    sectionTable getProperty(stage _phase)
    {
        double A_tf,A_web,A_bf,A_slab,A_rf;
        double y_tf,y_web,y_bf,y_slab,y_rf;
        double Ay_tf,Ay_web,Ay_bf,Ay_slab,Ay_rf;
        double Ay2_tf,Ay2_web,Ay2_bf,Ay2_slab,Ay2_rf;
        double Io_tf,Io_web,Io_bf,Io_slab,Io_rf;
        double I_tf,I_web,I_bf,I_slab,I_rf;
        double Ay2,Ay,Io;

        double temp = w_slab;

         switch (_phase)
         {
         case stage::NONCOMPOSITE :
            {w_slab = 0;break;}
         case stage::COMPOSITE_SHORT :
            {w_slab = w_slab/m;break;}
         case stage::COMPOSITE_LONG :
            {w_slab = w_slab/(3*m);break;}
         case stage::COMPOSITE_REINFORCEMENT :
            {w_slab = 0;break;}


         }
        //Result
        sectionTable output;

        //Area of each component
        A_tf = w_tf*d_tf;
        A_web = w_web*d_web;
        A_bf = w_bf*d_bf;
        A_slab = w_slab*d_slab/m;

        if (_phase != stage::COMPOSITE_REINFORCEMENT)
            A_rf = 0;
        else
            A_rf = As;

        //CG of each component from bottom of the girder
        y_tf = d_bf + w_web + d_tf/2;
        y_web = d_bf + w_web/2;
        y_bf = d_bf/2;
        y_slab = d_bf + w_web + haunch + d_slab/2;
        y_rf = d_bf + w_web + haunch + (d_slab - 2.5 - 1.0)/2 + 1.0;

        //A*y of each component
        Ay_tf = A_tf*y_tf;
        Ay_web = A_web*y_web;
        Ay_bf = A_bf*y_bf;
        Ay_slab = A_slab*y_slab;
        Ay_rf = A_rf * y_rf;

        //Moment of Inertia of each component
        Io_tf = w_tf*d_tf*d_tf*d_tf/12;
        Io_web = d_web*w_web*w_web*w_web/12;
        Io_bf = w_bf*d_bf*d_bf*d_bf/12;
        if (_phase == stage::NONCOMPOSITE)
        Io_slab = w_slab*d_slab*d_slab*d_slab/12;
        Io_rf = 0;

        Io = Io_slab + Io_rf + Io_tf + Io_web + Io_bf;

        // Total Area of the section
        A = A_slab + A_tf + A_web + A_bf + A_rf;
        Ay = Ay_slab + Ay_tf + Ay_web + Ay_bf + Ay_rf;

        // Centroid of the section
        y = Ay/A;


        // Moment of Inertia with respect to CG
        Ay2_slab = A_slab * std::pow(y-y_slab,2);
        Ay2_tf =  A_tf* std::pow(y-y_tf,2);
        Ay2_web = A_web* std::pow(y-y_web,2);
        Ay2_bf = A_bf* std::pow(y-y_bf,2);
        Ay2_rf =  A_rf* std::pow(y-y_rf,2);

        Ay2 = Ay2_slab + Ay2_tf + Ay2_web + Ay2_bf + Ay2_rf;

        I_slab = Io_slab + Ay2_slab ;
        I_tf = Io_tf + Ay2_tf;
        I_web = Io_web + Ay2_web;
        I_bf = Io_bf + Ay2_bf;
        I_rf = Io_rf + Ay2_rf;

        // Total Moment of Inertia of the section

        I = I_slab + I_rf + I_tf + I_web + I_bf;

        // Output structure
        output.I = I;
        output.Io = Io;
        output.A = A;
        output.y = y;
        output.Ay = Ay;

        output.I_slab = I_slab; output.I_rf = I_rf; output.I_tf = I_tf; output.I_web = I_web; output.I_bf = I_bf;

        output.Io_slab = Io_slab; output.Io_rf = Io_rf; output.Io_tf = Io_tf; output.Io_web = Io_web; output.Io_bf = Io_bf;

        output.Ay_slab = Ay_slab; output.Ay_rf = Ay_rf; output.Ay_tf = Ay_tf; output.Ay_web = Ay_web; output.Ay_bf = Ay_bf;

        output.Ay2_slab = Ay2_slab; output.Ay2_rf = Ay2_rf; output.Ay2_tf = Ay2_tf; output.Ay2_web = Ay2_web; output.Ay2_bf = Ay2_bf;

        output.y_slab = y_slab; output.y_rf = y_rf; output.y_tf = y_tf; output.y_web = y_web; output.y_bf = y_bf;

        output.A_slab = A_slab; output.A_rf = A_rf; output.A_tf = A_tf; output.A_web = A_web; output.A_bf = A_bf;

        if (_phase == stage::NONCOMPOSITE || _phase == stage::COMPOSITE_REINFORCEMENT)
        {
                output.w_slab = 0;
                output.d_slab = 0;
        }
        else
        {
             output.w_slab = w_slab;
             output.d_slab = d_slab;
        }

        output.w_tf = w_tf; output.d_tf = d_tf;
        output.w_web = w_web; output.d_web = d_web;
        output.w_bf = w_bf; output.d_bf = d_bf;


        w_slab = temp;

        return output;
    }

};

