////////////////////////////////////////////////////////////////////////////////
// File:        main_genrandoms.cc
// Class:       MAT320
// Asssignment: Project 3
// Author:      Connor Deakin
// Email:       connor.deakin@digipen.edu
// Date:        2018-11-04
////////////////////////////////////////////////////////////////////////////////

#include <iostream>

#include "Complex.h"

int main(int argc, char * argv[])
{
    if(argc != 2)
    {
        std::cout << "Error: Incorrect number of arguments. \n"
                     "genrandoms.exe N"
                  << std::endl;
        return 0;
    }

    unsigned values = (unsigned)atoi(argv[1]);
    for(unsigned i = 0; i < values; ++i)
    {
        Complex c;
        c.Random(-10, 10);
        std::cout << c << std::endl;
    }

    return 0;
}
