#include "plate_girder_design.h"
#include <cstdlib>

void plate_girder_design::report()
{
    // Open Document
    out.open("Plate_Girder_Design.tex");
    out << "\\documentclass[11pt]{book}" << std::endl;
    out << "\\usepackage{geometry}" << std::endl;
    out << "\\usepackage{float}" << std::endl;
    out << "\\geometry{" << std::endl;
    out << "letterpaper," << std::endl;
    out << "left=1.0in, " << std::endl;
    out << "top=1.0in," << std::endl;
    out << "right=0.5in," << std::endl;
    out << "bottom=0.5in" << std::endl;
    out << "}" << std::endl;
    out << "\\begin{document}" <<std::endl;


    // Data Functions

    //Calculation of Section Properties
    repSecProp();



    // Close Document
    out << "\\end{document}" << std::endl;
    out.close();

    // Create PDF
    createPDF();


}


void plate_girder_design::createPDF()
{
    std::cout << "Generating PDF" << std::endl;
     #if defined(_WIN32)
   // this is a Windows environment!
    std::cout << "Windows Environment" << std::endl;
    system("pdflatex Plate_Girder_Design.tex");

     #elif defined(__APPLE__)
   // this is a Mac OSX environment!
    std::cout << "MACOS Environment" << std::endl;
    system("/Library/TeX/texbin/pdflatex Plate_Girder_Design.tex");

    #endif // defined

    }


void plate_girder_design::repSecProp()
{

    for (auto i:sectionData_)
    {
        out << "\\begin{table}[H]" <<std::endl;
        out << "\\centering" <<std::endl;
        out << "\\caption{Girder Only}";
        out << "\\begin{tabular}{|c|c|c|c|c|c|c|c|c|}" << std::endl;
        out << "\\hline" << std::endl;
        out << "Shape &Width(b)     &Depth(d)  &Area(A)   &CG($y_c$)    &A$d$     &$I_o$      &$Ay_c^2$ &I      \\\\" << std::endl;
        out << "      &in           &in        &$in^2$    &in           &$in^3$     &$in^4$   &$in^4$   &$in^4$  \\\\" << std::endl;
        out << "\\hline" << std::endl;
        out << std::fixed << std::setprecision(2);


    /*
    double w_tf,w_web,w_bf,w_slab;
    double d_tf,d_web,d_bf,d_slab;
    double A_tf,A_web,A_bf,A_slab,A_rf;
    double y_tf,y_web,y_bf,y_slab,y_rf;
    double Ay_tf,Ay_web,Ay_bf,Ay_slab,Ay_rf;
    double Io_tf,Io_web,Io_bf,Io_slab,Io_rf;
    double I_tf,I_web,I_bf,I_slab,I_rf;
    double m;
    double Ay,A,y,Io,I;
    */


    out << "Top Flange "
                    << " &" << i.getProperty(stage::NONCOMPOSITE).w_tf
                    << " &" << i.getProperty(stage::NONCOMPOSITE).d_tf
                    << " &" << i.getProperty(stage::NONCOMPOSITE).A_tf
                    << " &" << i.getProperty(stage::NONCOMPOSITE).y_tf
                    << " &" << i.getProperty(stage::NONCOMPOSITE).Ay_tf
                    << " &" << i.getProperty(stage::NONCOMPOSITE).Io_tf
                    << " &" << i.getProperty(stage::NONCOMPOSITE).Ay2_tf
                    << " &" << i.getProperty(stage::NONCOMPOSITE).I_tf
                    << "\\\\ \\hline" << std::endl;

    out << "Web "
                    << " &" << i.getProperty(stage::NONCOMPOSITE).w_web
                    << " &" << i.getProperty(stage::NONCOMPOSITE).d_web
                    << " &" << i.getProperty(stage::NONCOMPOSITE).A_web
                    << " &" << i.getProperty(stage::NONCOMPOSITE).y_web
                    << " &" << i.getProperty(stage::NONCOMPOSITE).Ay_web
                    << " &" << i.getProperty(stage::NONCOMPOSITE).Io_web
                    << " &" << i.getProperty(stage::NONCOMPOSITE).Ay2_web
                    << " &" << i.getProperty(stage::NONCOMPOSITE).I_web
                    << "\\\\ \\hline" << std::endl;

    out << "Bottom Flange "
                    << " &" << i.getProperty(stage::NONCOMPOSITE).w_bf
                    << " &" << i.getProperty(stage::NONCOMPOSITE).d_bf
                    << " &" << i.getProperty(stage::NONCOMPOSITE).A_bf
                    << " &" << i.getProperty(stage::NONCOMPOSITE).y_bf
                    << " &" << i.getProperty(stage::NONCOMPOSITE).Ay_bf
                    << " &" << i.getProperty(stage::NONCOMPOSITE).Io_bf
                    << " &" << i.getProperty(stage::NONCOMPOSITE).Ay2_bf
                    << " &" << i.getProperty(stage::NONCOMPOSITE).I_bf
                    << "\\\\ \\hline" << std::endl;

    out << " "
                        << " &" << "-"
                        << " &" << "-"
                        << " &" << i.getProperty(stage::NONCOMPOSITE).A
                        << " &" << i.getProperty(stage::NONCOMPOSITE).y
                        << " &" << i.getProperty(stage::NONCOMPOSITE).Ay
                        << " &" << i.getProperty(stage::NONCOMPOSITE).Io
                        << " &" << i.getProperty(stage::NONCOMPOSITE).Ay2
                        << " &" << i.getProperty(stage::NONCOMPOSITE).I
                        << "\\\\ \\hline" << std::endl;




        out << "\\end{tabular}" << std::endl;
        out << "\\end{table}" << std::endl;

    }








    /*
    int num = 0;
    int s = 1;
    for (auto i:secProp)
    {
        out << "\\begin{table}[H]" <<std::endl;
        out << "\\centering" <<std::endl;
        if (i.flag == secPropType::GIRDERONLY)
            out << "\\caption{Girder Only}";
        else if(i.flag == secPropType::COMPOSITE_SHORT)
            out << "\\caption{Composite Section (Short Term)}";
        else if(i.flag == secPropType::COMPOSITE_LONG)
            out << "\\caption{Composite Section (Long Term)}";
        else if(i.flag == secPropType::COMPOSITE_RF)
            out << "\\caption{Composite Section (Reinforcement Only)}";

        out << "\\begin{tabular}{|c|c|c|c|c|c|c|c|}" << std::endl;
        out << "\\hline" << std::endl;
        out << "Shape &Width(b)     &Depth(d)  &Area(A)   &CG($y_c$)    &A$y_c$     &$I_o$    &I      \\\\" << std::endl;
        out << "      &in           &in        &$in^2$    &in           &$in^3$     &$in^4$   &$in^4$  \\\\" << std::endl;
        out << "\\hline" << std::endl;
        out << std::fixed << std::setprecision(2);
        for (auto j:i.data)
        {
            switch(s)
            {
                case 1:
                {
                    out << "Top Flange "
                    << " &" << tf_width[num]
                    << " &" << tf_thk[num]
                    << " &" << j.A
                    << " &" << j.y
                    << " &" << j.Ay
                    << " &" << j.Io
                    << " &" << j.I
                    << "\\\\ \\hline" << std::endl;
                    break;
                }
                case 2:
                {
                    out << "Web "
                    << " &" << w_width[num]
                    << " &" << w_thk[num]
                    << " &" << j.A
                    << " &" << j.y
                    << " &" << j.Ay
                    << " &" << j.Io
                    << " &" << j.I
                    << "\\\\ \\hline" << std::endl;
                    break;
                }
                case 3:
                {
                    out << "Bottom Flange "
                    << " &" << bf_width[num]
                    << " &" << bf_thk[num]
                    << " &" << j.A
                    << " &" << j.y
                    << " &" << j.Ay
                    << " &" << j.Io
                    << " &" << j.I
                    << "\\\\ \\hline" << std::endl;
                    break;
                }
                case 4:
                {
                    if (i.flag==secPropType::COMPOSITE_RF)
                    {
                        out << "Reinforcement"
                        << " &" << "-"
                        << " &" << "-"
                        << " &" << j.A
                        << " &" << j.y
                        << " &" << j.Ay
                        << " &" << j.Io
                        << " &" << j.I
                        << "\\\\ \\hline" << std::endl;
                    }
                    else
                    {
                        out << "Slab"
                        << " &" << bEff[num]
                        << " &" << slab_thk[num] - int_ws
                        << " &" << j.A
                        << " &" << j.y
                        << " &" << j.Ay
                        << " &" << j.Io
                        << " &" << j.I
                        << "\\\\ \\hline" << std::endl;
                    }

                    break;
                }
                default:
                {
                    break;
                }
            }
            s++;
            }
            out << " "
                        << " &" << "-"
                        << " &" << "-"
                        << " &" << i.A
                        << " &" << "-"
                        << " &" << "-"
                        << " &" << "-"
                        << " &" << i.I
                        << "\\\\ \\hline" << std::endl;
        s = 1;
        num++;

        if (num==nSec)
            num = 0;
        out << "\\end{tabular}" << std::endl;
        out << "\\end{table}" << std::endl;

    }

*/
}
