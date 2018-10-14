////////////////////////////////////////////////////////////////////////////////
// File:        main_dft.cc
// Class:       MAT320
// Asssignment: Project 2
// Author:      Connor Deakin
// Email:       connor.deakin@digipen.edu
// Date:        2018-10-12
////////////////////////////////////////////////////////////////////////////////

#include <iostream>

#include "Complex.h"

// DFT & InverseDFT ////////////////////////////////////////////////////////////

#define TAU 6.28318530717958647692

void DFT(std::vector<Complex> * v_output, 
         const std::vector<Complex> & v_input, 
         unsigned num_samples)
{
    for(unsigned k = 0; k < num_samples; ++k)
    
    {
        Complex output(0.0, 0.0);
        for(unsigned n = 0; n < num_samples; ++n)
        {
            Complex addend;
            Complex unity_root;
            unity_root.Polar(1.0, (-TAU * k * n) / num_samples);
            addend = v_input[n] * unity_root; 
            output += addend;
        }
        v_output->push_back(output);
    }
}

void InverseDFT(std::vector<Complex> * v_output, 
         const std::vector<Complex> & v_input, 
         unsigned num_samples)
{

    for(unsigned k = 0; k < num_samples; ++k)
    
    {
        Complex output(0.0, 0.0);
        for(unsigned n = 0; n < num_samples; ++n)
        {
            Complex addend;
            Complex unity_root;
            unity_root.Polar(1.0, (TAU * k * n) / num_samples);
            addend = v_input[n] * unity_root; 
            output += addend;
        }
        v_output->push_back(output / num_samples);
    }
}

// Main ////////////////////////////////////////////////////////////////////////

int main(int argc, char ** argv)
{
    if(argc < 3)
    {
        std::cout << "Error: Incorrect number of arguments.\n"
                     "dft.exe N filename"
                  << std::endl;
        return 0;
    }

    unsigned samples = (unsigned)atoi(argv[1]);
    std::string filename(argv[2]);

    std::vector<Complex> v_input;
    Complex::FileToVComplex(&v_input, filename);

    if(samples > v_input.size())
    {
        std::cout << "Error: The N value provided is greater than the number "
                     "of complex numbers in the given file."
                  << std::endl;
        return 0;
    }

    std::vector<Complex> v_output;
    DFT(&v_output, v_input, samples);

    for(const Complex & c: v_output)
    {
        std::cout << c << std::endl;
    }
}

