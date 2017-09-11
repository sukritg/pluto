#include "inputFileReader.h"

inputFileReader::inputFileReader(std::string _fileName)
{
    fileName = _fileName;
}

inputFileReader::~inputFileReader()
{
    //dtor
}


void inputFileReader::read()
{
    reader in(fileName);
    std::vector<std::string> line;
    int n = 0;
    std::cout << "START" << std::endl;
    while ((line = in.readLine())[0] != "END")
    {
        std::cout << n++ << "\t";
        tokenAction(tokenCheck(line),line);
    }

}


token inputFileReader::tokenCheck(std::vector<std::string> line)
{
    // Converts string to upper case
    for (unsigned int i=0; i<line[0].size();i++)
    {
        line[0].at(i) = std::toupper(line[0].at(i));
    }
    // Check for possible tokens
    if(line[0]== "BEAM_CC_SPACING")
        return token::BEAM_CC_SPACING;
    else if(line[0]== "BEAM_OVR_LENGTH")
        return token::BEAM_OVR_LENGTH;
    else if(line[0]== "NUM_BEAMS")
        return token::NUM_BEAMS;
    else if(line[0]== "COVER_TOP")
        return token::COVER_TOP;
    else if(line[0]== "COVER_BOTTOM")
        return token::COVER_BOTTOM;
    else if(line[0]== "PERM_STRESS_C")
        return token::PERM_STRESS_C;
    else if(line[0]== "PERM_STRESS_S")
        return token::PERM_STRESS_S;
    else if(line[0]== "WT_SW")
        return token::WT_SW;
    else if(line[0]== "WT_FWS")
        return token::WT_FWS;
    else if(line[0]== "WT_PARAPET")
        return token::WT_PARAPET;
    else if(line[0]== "WT_SIP")
        return token::WT_SIP;
    else if(line[0]== "SLAB_THICKNESS")
        return token::SLAB_THICKNESS;
    else if(line[0]== "SECTION")
        return token::SECTION;
    else
        return token::NULL_TOKEN;
}

void inputFileReader::tokenAction(token tok,std::vector<std::string> &line)
{
    switch(tok)
    {
    case token::BEAM_CC_SPACING:
        {
            std::cout<< "Beam C/C spacing found" <<std::endl;
            BEAM_CC_SPACING = toDouble(line[1]);
            break;
        }
    case token::BEAM_OVR_LENGTH:
        {
            std::cout<< "Beam overhang length found" <<std::endl;
            BEAM_OVR_LENGTH = toDouble(line[1]);
            break;
        }
    case token::NUM_BEAMS:
        {
            std::cout<< "Number of girders found" <<std::endl;
            NUM_BEAMS = toDouble(line[1]);
            break;
        }
    case token::COVER_TOP:
        {
            std::cout<< "Concrete top cover found" <<std::endl;
            COVER_TOP = toDouble(line[1]);
            break;
        }
    case token::COVER_BOTTOM:
        {
            std::cout<< "Concrete bottom cover found" <<std::endl;
            COVER_BOTTOM = toDouble(line[1]);
            break;
        }
    case token::PERM_STRESS_C:
        {
            std::cout<< "Permissible compressive stress in concrete found" <<std::endl;
            PERM_STRESS_C = toDouble(line[1]);
            break;
        }
    case token::PERM_STRESS_S:
        {
            std::cout<< "Permissible tensile stress in steel found" << std::endl;
            PERM_STRESS_S = toDouble(line[1]);
            break;
        }
    case token::WT_SW:
        {
            std::cout<< "Unit weight of concrete found" << std::endl;
            WT_SW = toDouble(line[1]);
            break;
        }
    case token::WT_FWS:
        {
            std::cout<< "Unit weight of future wearing surface found" <<std::endl;
            WT_FWS = toDouble(line[1]);
            DIST_FWS_LEFT = toDouble(line[2]);
            DIST_FWS_RIGHT = toDouble(line[3]);
            break;
        }
    case token::WT_PARAPET:
        {
            std::cout<< "Unit weight of parapet found" <<std::endl;
            WT_PARAPET = toDouble(line[1]);
            DIST_PARAPET_LEFT = toDouble(line[2]);
            DIST_PARAPET_RIGHT = toDouble(line[3]);
            break;
        }
    case token::WT_SIP:
        {
            std::cout<< "Unit weight of stay-in-place found" <<std::endl;
            WT_SIP = toDouble(line[1]);
            break;
        }
    case token::SLAB_THICKNESS:
        {
            std::cout<< "Slab thickness found" <<std::endl;
            SLAB_THICKNESS = toDouble(line[1]);
            break;
        }
    case token::SECTION:
        {
            std::cout << "section found" << std::endl;
            Area = toDouble(line[1]);
            Inertia = toDouble(line[2]);
        }
    case token::NULL_TOKEN:
        {
            std::cout << "Null token found" << std::endl;
            break;
        }

    }
}
