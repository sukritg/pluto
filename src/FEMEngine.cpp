#include "FEMEngine.h"

void FEMEngine::  start    (const std::vector<node> &_nodalData,
                            const std::vector<element> &_elementData,
                            const std::vector<bConditions> &_bcData,
                            const std::vector<lCombination> &_loadComboData)
{
    nodalData = _nodalData;
    elementData = _elementData;
    bcData = _bcData;
    loadComboData = _loadComboData;
}


void FEMEngine::run()
{
    createIDMatrix();
    createStiffMatrix();
    createForceMatrix();
    applyBC();
    solve();
    populateNodalResData();
    calcElmForces();
    populateElementResData();
}

void FEMEngine::print()
{
    printNodeTable();
    printElementTable();
}

void FEMEngine::changeElementData(const std::vector<element> &_elementData)
{
    elementData = _elementData;
}

void FEMEngine ::createIDMatrix()
{
    IDArray.setSize(int(nodalData.size()),3); // 3 DOF per node
    IDArray.setName("ID Array");
    for (unsigned int i=0; i<nodalData.size(); i++)
    {
        for (unsigned int j=0; j<bcData.size(); j++)
        {
            if (nodalData[i].getID()==bcData[j].getID())
            {
                for (int k=0; k<3; k++)
                {
                    IDArray(i+1,k+1) = bcData[j].getProperty()[k];

                }
            }
        }
    }

    int c=1;
    for (unsigned int i=1; i<=IDArray.rowSize(); i++)
    {
        for (unsigned int j=1; j<=IDArray.colSize(); j++)
        {
            if (IDArray(i,j)== 0)
            {
                IDArray(i,j) = c;
                c++;
            }
            else
            {
                IDArray(i,j) = -1;
            }
        }
    }

    for (unsigned int i=1; i<=IDArray.rowSize(); i++)
    {
        for (unsigned int j=1; j<=IDArray.colSize(); j++)
        {
            if (IDArray(i,j)== -1)
            {
                IDArray(i,j) = c;
                c++;
            }
        }
    }

// To print ID Matrix
/*
        for (int i=1; i<=IDArray.rowSize();i++)
        {
            for (int j=1; j<=IDArray.colSize();j++)
            {
                    std::cout <<IDArray(i,j) << "\t";
            }
            std::cout << std::endl;
        }
*/

}

void FEMEngine::createStiffMatrix()
{
    writer out("output.txt");

    K.setName("Stiffness Matrix");
    K.setSize(nodalData.size()*3,nodalData.size()*3);K.setValue(0);

    for (unsigned int i=0; i<elementData.size(); i++)
    {
        matrix ke(6,6);
        matrix L(6,6);
        double A,E,I,le,l,m;
        std::vector<int> ii {0,0,0,0,0,0};
        int SN = elementData[i].getNodes()[0].getID();
        int EN = elementData[i].getNodes()[1].getID();
        A = elementData[i].getSection().getProperty()[0];
        I = elementData[i].getSection().getProperty()[1];
        E = elementData[i].getMaterial().getProperty()[0];
        le = std::sqrt(std::pow(elementData[i].getNodes()[1].getProperty()[0] - elementData[i].getNodes()[0].getProperty()[0],2)
                       + std::pow(elementData[i].getNodes()[1].getProperty()[1] - elementData[i].getNodes()[0].getProperty()[1],2));
        l = (elementData[i].getNodes()[1].getProperty()[0] - elementData[i].getNodes()[0].getProperty()[0])/le;
        m = (elementData[i].getNodes()[1].getProperty()[1] - elementData[i].getNodes()[0].getProperty()[1])/le;

        ke(1,1) = A*E/le;
        ke(1,4) = -ke(1,1);
        ke(2,2) = 12*E*I/(le*le*le);
        ke(2,3) = 6*E*I/(le*le);
        ke(2,5) = -ke(2,2);
        ke(2,6) = ke(2,3);
        ke(3,2) = 6*E*I/(le*le);
        ke(3,3) = 4*E*I/le;
        ke(3,5) = -ke(3,2);
        ke(3,6) = 2*E*I/le;
        ke(4,1) = -A*E/le;
        ke(4,4) = -ke(4,1);
        ke(5,2) = -12*E*I/(le*le*le);
        ke(5,3) = -6*E*I/(le*le);
        ke(5,5) = -ke(5,2);
        ke(5,6) = ke(5,3);
        ke(6,2) = 6*E*I/(le*le);
        ke(6,3) = 2*E*I/le;
        ke(6,5) = -ke(6,2);
        ke(6,6) = 4*E*I/le;

        L(1,1) = l;
        L(1,2) = m;
        L(2,1) = -m;
        L(2,2) = l;
        L(3,3) = 1;
        L(4,4) = l;
        L(4,5) = m;
        L(5,4) = -m;
        L(5,5) =l;
        L(6,6) = 1;

        for (int j=0; j<3; j++)
            ii[j] = IDArray(SN,j+1);

        for (int j=3; j<6; j++)
            ii[j] = IDArray(EN,j-2);


        ke = ~L * ke * L;

        int r=1;
        int c=1;
        for (int j:ii)
        {
            for (int k:ii)
            {
                K(j,k) = K(j,k) + ke(r,c);
                c++;
            }
            r++;
            c=1;
        }

    }

//To Print
out.writeMatrix(K,10,true);
out.close();

}

void FEMEngine::createForceMatrix()
{
writer out("output.txt");
F.setName("Force Matrix");
F.setSize(nodalData.size()*3,loadComboData.size()); F.setValue(0.0);
std::vector<int> ii {0,0,0,0,0,0};
 force.setSize(elementData.size(),6);

for (unsigned int i=0; i < loadComboData.size(); i++)
{
   force.setValue(0.0);
    for(unsigned int j=0; j < loadComboData[i].getProperty().size(); j++)
    {
        loadType type = loadComboData[i].getProperty()[j].getType();
        std::vector<int> elm = loadComboData[i].getProperty()[j].getElements();
        std::vector<double> values = loadComboData[i].getProperty()[j].getValues();

        switch (type)
            {
                case loadType::EC_Fy :
                {
                    matrix f(6,1); f.setName("local force matrix (f)");
                    matrix L(6,6);
                    for (unsigned int k=0; k<elm.size();k++)
                    {
                        element e = searchID(elementData,elm[k]);
                        double le = std::sqrt(std::pow(e.getNodes()[1].getProperty()[0] - e.getNodes()[0].getProperty()[0],2)
                       + std::pow(e.getNodes()[1].getProperty()[1] - e.getNodes()[0].getProperty()[1],2));
                        double l = (e.getNodes()[1].getProperty()[0] - e.getNodes()[0].getProperty()[0])/le;
                        double m = (e.getNodes()[1].getProperty()[1] - e.getNodes()[0].getProperty()[1])/le;

                        L(1,1) = l;         L(1,2) = m;
                        L(2,1) = -m;        L(2,2) = l;
                        L(3,3) = 1;
                        L(4,4) = l;        L(4,5) = m;
                        L(5,4) = -m;       L(5,5) =l;
                        L(6,6) = 1;

                        double a = values[1], b = le - a , P = values[0];
                        int SN = e.getNodes()[0].getID();
                        int EN = e.getNodes()[1].getID();

                        for (int j=0; j<3; j++)
                                ii[j] = IDArray(SN,j+1);
                        for (int j=3; j<6; j++)
                                ii[j] = IDArray(EN,j-2);

                        f(1,1) = 0; f(2,1) = P*b*b*(le+2*a)/(le*le*le); f(3,1) =  + P*a*b*b/(le*le);
                        f(4,1) = 0; f(5,1) = P*a*a*(le+2*b)/(le*le*le); f(6,1) =  - P*a*a*b/(le*le);

                        force(e.getID(),1) = force(e.getID(),1) + f(1,1);
                        force(e.getID(),2) = force(e.getID(),2) + f(2,1);
                        force(e.getID(),3) = force(e.getID(),3) + f(3,1);
                        force(e.getID(),4) = force(e.getID(),4) + f(4,1);
                        force(e.getID(),5) = force(e.getID(),5) + f(5,1);
                        force(e.getID(),6) = force(e.getID(),6) + f(6,1);


                        matrix fr = ~L * f;
                        int pos = 1;
                        for (auto dof:ii)
                        {
                            F(dof,i+1) = F(dof,i+1) + fr(pos,1);
                            pos++;
                        }

                    }
                    break;
                }
                case loadType::EU_Fy :
                {
                    matrix f(6,1); f.setName("local force matrix (f)");
                    matrix L(6,6);
                    for (unsigned int k=0; k<elm.size();k++)
                    {
                        element e = searchID(elementData,elm[k]);
                        double le = std::sqrt(std::pow(e.getNodes()[1].getProperty()[0] - e.getNodes()[0].getProperty()[0],2)
                       + std::pow(e.getNodes()[1].getProperty()[1] - e.getNodes()[0].getProperty()[1],2));
                        double l = (e.getNodes()[1].getProperty()[0] - e.getNodes()[0].getProperty()[0])/le;
                        double m = (e.getNodes()[1].getProperty()[1] - e.getNodes()[0].getProperty()[1])/le;

                        L(1,1) = l;         L(1,2) = m;
                        L(2,1) = -m;        L(2,2) = l;
                        L(3,3) = 1;
                        L(4,4) = l;        L(4,5) = m;
                        L(5,4) = -m;       L(5,5) =l;
                        L(6,6) = 1;

                        int SN = e.getNodes()[0].getID();
                        int EN = e.getNodes()[1].getID();

                        if (values.size()==1)
                        {
                            double w = values[0];
                            f.setValue(0.0);
                            f(1,1) = 0; f(2,1) = w*le/2; f(3,1) = w*le*le/12;
                            f(4,1) = 0; f(5,1) = w*le/2; f(6,1) = -w*le*le/12;


                        }
                        else //parameters should be 3 - intensity, start load dist. from left, end load dist. from left
                        {
                            double w = values[0], a = values[1], b = values[2];
                            double x1 = a, x2 = b;

                            double shape1_x1 = x1 - (x1*x1*x1)/(le*le) + (x1*x1*x1*x1)/(2*le*le*le);
                            double shape2_x1 = (x1*x1)/2 - (2*x1*x1*x1)/(3*le) + (x1*x1*x1*x1)/(4*le*le);
                            double shape3_x1 =   (x1*x1*x1)/(le*le) - (x1*x1*x1*x1)/(2*le*le*le);
                            double shape4_x1 = - (x1*x1*x1)/(3*le)  + (x1*x1*x1*x1)/(4*le*le);

                            double shape1_x2 = x2 - (x2*x2*x2)/(le*le) + (x2*x2*x2*x2)/(2*le*le*le);
                            double shape2_x2 = (x2*x2)/2 - (2*x2*x2*x2)/(3*le) + (x2*x2*x2*x2)/(4*le*le);
                            double shape3_x2 =   (x2*x2*x2)/(le*le) - (x2*x2*x2*x2)/(2*le*le*le);
                            double shape4_x2 = - (x2*x2*x2)/(3*le)  + (x2*x2*x2*x2)/(4*le*le);

                            f.setValue(0.0);
                            f(1,1) = 0;
                            f(2,1) = w*(shape1_x2-shape1_x1);
                            f(3,1) = w*(shape2_x2-shape2_x1);
                            f(4,1) = 0;
                            f(5,1) = w*(shape3_x2-shape3_x1);
                            f(6,1) = w*(shape4_x2-shape4_x1);

                        }

                            force(e.getID(),1) = force(e.getID(),1) + f(1,1);
                            force(e.getID(),2) = force(e.getID(),2) + f(2,1);
                            force(e.getID(),3) = force(e.getID(),3) + f(3,1);
                            force(e.getID(),4) = force(e.getID(),4) + f(4,1);
                            force(e.getID(),5) = force(e.getID(),5) + f(5,1);
                            force(e.getID(),6) = force(e.getID(),6) + f(6,1);

                        for (int j=0; j<3; j++)
                                ii[j] = IDArray(SN,j+1);
                            for (int j=3; j<6; j++)
                                ii[j] = IDArray(EN,j-2);
                        matrix fr = ~L * f;

                        int pos = 1;
                        for (auto dof:ii)
                        {
                            F(dof,i+1) = F(dof,i+1) + fr(pos,1);
                            pos++;
                        }
                    }
                    break;
                }
            }



    }
    forces.push_back(force);
    force.setValue(0.0);
}
//TO print
out.writeMatrix(F,15,true);
out.close();
}

void FEMEngine::applyBC()
{
    writer out("output.txt");
    Kr.setName("Reduced Stiffness Matrix");
    Fr.setName("Reduced Force Matrix");
    unsigned int ndof = 3*nodalData.size();
    unsigned int jpos = ndof;
    for (unsigned int i = 0; i<bcData.size(); i++)
    {
        for (unsigned int j=0; j<bcData[i].getProperty().size(); j++)
        {
            if (bcData[i].getProperty()[j] == 1)
            {
                jpos--;
            }
        }
    }
    Kr.setSize(jpos,jpos);
    Fr.setSize(jpos,loadComboData.size());

    for (unsigned int i=1; i<=Kr.rowSize();i++)
    {
        for (unsigned int j=1; j<=Kr.colSize();j++)
        {
            Kr(i,j) = K(i,j);
        }
    }


    for (unsigned int i=1; i<=Fr.rowSize();i++)
    {
        for (unsigned int j=1; j<=Fr.colSize();j++)
        {
            Fr(i,j) = F(i,j);
        }
    }
    out.writeMatrix(Kr,10,true);
    out.writeMatrix(Fr,10,true);
    out.close();
}

void FEMEngine::solve()
{
    unsigned int ndof = 3*nodalData.size();
    unsigned int jpos = ndof;
    for (unsigned int i = 0; i<bcData.size(); i++)
    {
        for (unsigned int j=0; j<bcData[i].getProperty().size(); j++)
        {
            if (bcData[i].getProperty()[j] == 1)
            {
                jpos--;
            }
        }
    }
    writer out("output.txt");
    d.setName("Displacement Matrix");
    d.setSize(jpos,loadComboData.size());
    mathlib solver;
    matrix Fred(jpos,1,"Force Vector");

    for (unsigned int i=0; i< loadComboData.size(); i++)
    {
        for (unsigned int j=1; j<=Fr.rowSize();j++)
        {
            Fred(j,1) = Fr(j,i+1);
        }
        matrix dt = solver.choleskySolve(Kr,Fred);

        for (unsigned int k=1; k<=dt.rowSize();k++)
        {
                d(k,i+1) = dt(k,1);
        }
    }

    matrix dt(ndof,loadComboData.size()); dt.setValue(0.0);
    dt.setName("Displacement Matrix");
    for (unsigned int i = 1; i<=d.rowSize();i++)
    {
        for (unsigned int j=1; j<=d.colSize(); j++)
        {
            dt(i,j) = d(i,j);
        }
    }
    d = dt;
/*
    matrix rt(ndof,1);
    matrix ft(ndof,1);
    dt.setSize(ndof,1);
    R.setSize(ndof,loadComboData.size()); R.setName("Reaction Matrix"); R.setValue(0.0);
    for (unsigned int i = 0; i<loadComboData.size();i++)
    {
        for (int j = 0; j<ndof; j++)
        {
             ft(j+1,1) = F(j+1,i+1);
        }
        for (int j = 0; j<ndof; j++)
        {
             dt(j+1,1) = d(j+1,i+1);
        }
        rt = K*dt - ft;

        for (unsigned int k=1; k<=dt.rowSize();k++)
        {
                R(k,i+1) = rt(k,1);
        }

    }
    out.writeMatrix(R,15);
*/
    R = K*d - F;
    R.setName("Reaction Matrix");
    out.writeMatrix(R,15);


    matrix A;
    A.setSize(5,5);A.setValue(0.0);
    A(1,1) =  3.5120; A(1,2) =  0.7679;
    A(2,1) =  0.7679; A(2,2) = 3.1520; A(2,4) =  -2;
    A(3,3) = 3.5120; A(3,4) =  -0.7679; A(3,5) =  0.7679;
    A(4,2) =  -2; A(4,3) = -0.7679; A(4,4) =  3.1520; A(4,5) = -1.1520;
    A(5,3) = 0.7679; A(5,4) =  -1.1520; A(5,5) = 3.1520;



    matrix B;
    B.setSize(5,1);
    B(4,1) =  -0.04;


  //  out.writeMatrix(A,10,false);
  //  out.writeMatrix(B,10,false);
   // out.writeText("Gauss Elimination Method");
   // out.writeMatrix(solver.gaussSolve(Kr,Fred),10);
   // out.writeText("Cholesky LLT Method");
   // out.writeMatrix(solver.choleskySolve(Kr,Fred),10);
   // out.writeText("LU Decomposition Method");
    //out.writeMatrix(solver.luSolve(Kr,Fred),10);




    out.writeMatrix(d,15,true);
    out.close();
}

void FEMEngine::populateNodalResData()
{
    unsigned int ndof = 3*nodalData.size();
    unsigned int jpos = ndof;
    for (unsigned int i = 0; i<bcData.size(); i++)
    {
        for (unsigned int j=0; j<bcData[i].getProperty().size(); j++)
        {
            if (bcData[i].getProperty()[j] == 1)
            {
                jpos--;
            }
        }
    }

    writer out ("output.txt");
    out.writeMatrix(d,20);
    NodalResponse nr;
    std::vector<double> values1,values2;
    std::vector<NodalResponse> ntemp;


    for (unsigned int i=0; i< loadComboData.size() ; i++)
    {

        for (unsigned int j=0; j< nodalData.size(); j++)
        {
           for (unsigned int k=1; k <= IDArray.colSize(); k++)
            {
                values1.push_back(d(IDArray(j+1,k),i+1));
                values2.push_back(R(IDArray(j+1,k),i+1));
                //std::cout << "LC = " << i+1 << " Node No. = " << j+1 << " DOF = " << IDArray(j+1,k)
                //<< " value = " << d(IDArray(j+1,k),i+1) <<std::endl;
           }
            nr.setDsp(values1);
            nr.setReac(values2);
            ntemp.push_back(nr);
            values1.clear();
            values2.clear();
        }
        nodalRData.push_back(ntemp);
        ntemp.clear();
    }


//To print set print = 1
int print = 0;

if (print == 1)
{
  for (unsigned int i=0; i<nodalRData.size(); i++)
    {
    std::cout << "Load Case : " << i+1 << std::endl;
        for (unsigned int j=0; j < nodalRData[i].size(); j++)
        {
         std::cout << nodalRData[i][j].getDsp()[0] << "\t"
                    << nodalRData[i][j].getDsp()[1] << "\t"
                    << nodalRData[i][j].getDsp()[2] << "\t"
                    << std::endl;

        }
    }
}
}

void FEMEngine::calcElmForces()
{
    writer out("output.txt");
for (unsigned int lc = 0; lc <loadComboData.size(); lc++)
{
    ElmForce.setSize(elementData.size(),6);
    ElmForce.setName("Element Forces");
    for (unsigned int i=0; i< elementData.size(); i++)
    {
        matrix ke(6,6);
        matrix L(6,6);
        double A,E,I,le,l,m;
        std::vector<int> ii {0,0,0,0,0,0};
        matrix dsp(6,1);

        int SN = elementData[i].getNodes()[0].getID();
        int EN = elementData[i].getNodes()[1].getID();
        A = elementData[i].getSection().getProperty()[0];
        I = elementData[i].getSection().getProperty()[1];
        E = elementData[i].getMaterial().getProperty()[0];
        le = std::sqrt(std::pow(elementData[i].getNodes()[1].getProperty()[0] - elementData[i].getNodes()[0].getProperty()[0],2)
                       + std::pow(elementData[i].getNodes()[1].getProperty()[1] - elementData[i].getNodes()[0].getProperty()[1],2));
        l = (elementData[i].getNodes()[1].getProperty()[0] - elementData[i].getNodes()[0].getProperty()[0])/le;
        m = (elementData[i].getNodes()[1].getProperty()[1] - elementData[i].getNodes()[0].getProperty()[1])/le;

        ke(1,1) = A*E/le;
        ke(1,4) = -ke(1,1);
        ke(2,2) = 12*E*I/(le*le*le);
        ke(2,3) = 6*E*I/(le*le);
        ke(2,5) = -ke(2,2);
        ke(2,6) = ke(2,3);
        ke(3,2) = 6*E*I/(le*le);
        ke(3,3) = 4*E*I/le;
        ke(3,5) = -ke(3,2);
        ke(3,6) = 2*E*I/le;
        ke(4,1) = -A*E/le;
        ke(4,4) = -ke(4,1);
        ke(5,2) = -12*E*I/(le*le*le);
        ke(5,3) = -6*E*I/(le*le);
        ke(5,5) = -ke(5,2);
        ke(5,6) = ke(5,3);
        ke(6,2) = 6*E*I/(le*le);
        ke(6,3) = 2*E*I/le;
        ke(6,5) = -ke(6,2);
        ke(6,6) = 4*E*I/le;

        L(1,1) = l;
        L(1,2) = m;
        L(2,1) = -m;
        L(2,2) = l;
        L(3,3) = 1;
        L(4,4) = l;
        L(4,5) = m;
        L(5,4) = -m;
        L(5,5) =l;
        L(6,6) = 1;

        for (int j=0; j<3; j++)
            ii[j] = IDArray(SN,j+1);

        for (int j=3; j<6; j++)
            ii[j] = IDArray(EN,j-2);

        for (unsigned int j=0; j< ii.size();j++)
        {
            dsp(j+1,1) = d(ii[j],lc+1);
        }

        matrix fxr(6,1);
        for (int j=1; j<=6; j++)
            fxr(j,1) = forces[lc](elementData[i].getID(),j);
        matrix cal = ke*L*dsp - fxr;
        out.writeText("LC = " + toString(lc) + "\t Element = " + toString(elementData[i].getID()));
        out.writeMatrix(cal,15);

                for (int j=1; j<=6; j++)
                    ElmForce(i+1,j) = cal(j,1);

    }
    ElmForces.push_back(ElmForce);

}

    for (unsigned int i=0; i < ElmForces.size(); i++)
        out.writeMatrix(ElmForces[i],15);
    out.close();

}

 void FEMEngine::populateElementResData()
 {
     ElementResponse ER;
     std::vector<ElementResponse> ERVec;
     std::vector<double> values;
     std::cout << "Elm forces size = " << ElmForces.size() << std::endl;
     for (unsigned int lc = 0; lc < ElmForces.size(); lc++)
     {
         for (unsigned int e = 0; e < elementData.size(); e++)
         {
             for (int f = 1; f <= 6; f++)
             {
                values.push_back(ElmForces[lc](e+1,f));
             }
            ER.setID(elementData[e].getID());
            ER.setNodes(elementData[e].getNodes()[0].getID(),elementData[e].getNodes()[1].getID());
            ER.setProperty(values);
            values.clear();
            ERVec.push_back(ER);
         }
        elementRData.push_back(ERVec);
        ERVec.clear();
     }

 }

FEMEngine::FEMEngine()
{

}

FEMEngine::~FEMEngine()
{

}

element & FEMEngine::searchID(std::vector<element> &elData,int _id)
{
    for (unsigned int i = 0 ; i < elData.size(); i ++)
    {
        if (elData[i].getID() == _id)
            return elData[i];
    }
    std::cout << "Element Not Found!" << std::endl;
    return elData[0];
}

bool FEMEngine::searchElm_in_Load(load &_load, int _elmID)
{

    for (unsigned int i = 0; i < _load.getElements().size(); i++)
    {
        if (_load.getElements()[i] == _elmID)
            return true;
    }
    return false;
}

double FEMEngine::sectionForce(int _lc, int _elementID, double _dist, SFType _type)
{
    //Check
    if (_lc < 1 || _lc > loadComboData.size())
        std::cout << "Invalid requested load combination number" << std::endl;
    if (_elementID < 1 || _elementID > elementData.size())
        std::cout << "Invalid requested element ID number" << std::endl;
    if (_dist < 0 || _dist > 1)
        std::cout << "Invalid requested distance (0 < distance < 1)" << std::endl;


    double FEA = elementRData[_lc-1][_elementID-1].getProperty()[0];
    double FEV = elementRData[_lc-1][_elementID-1].getProperty()[1];
    double FEM = elementRData[_lc-1][_elementID-1].getProperty()[2];

        int i = _elementID-1;
        int SN = elementData[i].getNodes()[0].getID();
        int EN = elementData[i].getNodes()[1].getID();
        double le = std::sqrt(std::pow(elementData[i].getNodes()[1].getProperty()[0] - elementData[i].getNodes()[0].getProperty()[0],2)
                    + std::pow(elementData[i].getNodes()[1].getProperty()[1] - elementData[i].getNodes()[0].getProperty()[1],2));
    double axial = 0;
    double moment = 0;
    double shear = 0;

    axial = axial + FEA;
    shear = shear + FEV;
    moment  = moment + FEM - FEV * _dist*le;

    for (unsigned int i=0; i<loadComboData[_lc-1].getProperty().size(); i++)
    {
        // For concentrated load in Y direction
        if (loadComboData[_lc-1].getProperty()[i].getType() == loadType::EC_Fy
             && searchElm_in_Load(loadComboData[_lc-1].getProperty()[i],_elementID))
        {
            if (_dist*le > loadComboData[_lc-1].getProperty()[i].getValues()[1])
            {

                moment = moment - loadComboData[_lc-1].getProperty()[i].getValues()[0]
                                * (_dist*le - loadComboData[_lc-1].getProperty()[i].getValues()[1]);

                shear = shear +  loadComboData[_lc-1].getProperty()[i].getValues()[0];

            }
            else {/*Do nothing*/}

        }
        else if (loadComboData[_lc-1].getProperty()[i].getType() == loadType::EU_Fy
                     && searchElm_in_Load(loadComboData[_lc-1].getProperty()[i],_elementID))
            {
                std::vector<double> tmp = loadComboData[_lc-1].getProperty()[i].getValues();
            if (tmp.size() == 1)
            {
                moment = moment - loadComboData[_lc-1].getProperty()[i].getValues()[0] * _dist * _dist * 0.5*le*le;
                shear = shear + loadComboData[_lc-1].getProperty()[i].getValues()[0] * _dist*le;
            }
            else if(_dist*le > tmp[1] && _dist*le > tmp[2])
            {
                moment = moment - loadComboData[_lc-1].getProperty()[i].getValues()[0]
                * (tmp[2]-tmp[1]) * (_dist*le -  (tmp[1] + 0.5*(tmp[2]-tmp[1])));
                shear = shear + loadComboData[_lc-1].getProperty()[i].getValues()[0] * (tmp[2]-tmp[1]);
            }
            else if (_dist*le >= tmp[1] && _dist*le <= tmp[2])
            {
                moment = moment - loadComboData[_lc-1].getProperty()[i].getValues()[0]
                * (_dist*le-tmp[1]) * 0.5*(_dist*le-tmp[1]);
                shear = shear + loadComboData[_lc-1].getProperty()[i].getValues()[0] * (_dist*le-tmp[1]);

            }
            else {/*Do nothing*/}
            }
        else {/*Do nothing}*/}
}

    switch (_type)
    {
    case SFType::AXIAL :
        return axial; break;
    case SFType::SHEAR :
        return shear; break;
    case SFType::MOMENT :
        return moment; break;
    }
        return axial;

    return 0;
}
