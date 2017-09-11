#pragma once
#include <vector>

class section
{
    private:
    int sectionID;
    double Ax,Iz;
    std::vector<double> tf;
    
    
    public:
    void setProperty(std::vector<double> &data)
    {
        Ax = data[0];
        Iz = data[1];
    }

    std::vector<double> getProperty()
    {
        std::vector<double> data;
        data.push_back(Ax);
        data.push_back(Iz);
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
    void setTF(double _tf_width, double _tf_depth)
    {
        tf.clear();
        tf.push_back(_tf_width);
        tf.push_back(_tf_depth);
    }
    
    //optional: get top flange width of element
    std::vector<double> getTF()
    {
        return tf;
    }
    
    
};
