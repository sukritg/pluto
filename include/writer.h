#pragma once
#ifndef WRITER_H
#define WRITER_H

#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include "table.h"
#include "matrix.h"

class writer
{
    public:
        writer(std::string _fileName);
        ~writer();
        void writeTable(table &T, std::string title);
        void writeTableCopy(table &T, std::string title);
        void writeText (const std::string &text);
        void writeMatrix (const matrix &mat, int width);
        void writeMatrix (const matrix &mat, int width, bool sci);
        void close();
    protected:

    private:
        std::string fileName;
        std::ofstream out;
        unsigned int tableNo;
};

#endif // WRITER_H
