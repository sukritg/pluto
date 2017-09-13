#include <iostream>
#include "reader.h"
#include "plate_girder_design.h"
#include <fstream>
#include <stdlib.h>

int main(int argc,  char * argv[])
{
// Set Input file name if not provided as program arguments
std::string inputFile = "input.txt";

std::string module;
std::map<std::string,std::vector<double>> tokens;
    if (argc == 2)
    {
        reader inFile(argv[1]);
        inFile.configKeys();
        //inFile.printKeys();
        inFile.lexer();
        inFile.printTokenList();
        tokens = inFile.getTokens();
        module = inFile.getModuleName();

    }
    else if (argc == 1)
    {
        reader inFile(inputFile);
        inFile.configKeys();
        //inFile.printKeys();
        inFile.lexer();
        inFile.printTokenList();
        tokens = inFile.getTokens();
        module = inFile.getModuleName();
    }
    else
    {
        std::cout << "Invalid number of arguments" << std::endl;
    }

// Set Module with commands
   std::cout << "==========================================================="<< std::endl;
    if (module == "PLATE_GIRDER_DESIGN")
        {std::cout << "Starting Plate Girder Design" << std::endl; plate_girder_design workModule(tokens);}
    else
        std::cout << "No valid module found" << std::endl;
   std::cout << "==========================================================="<< std::endl;

    return 0;
}
