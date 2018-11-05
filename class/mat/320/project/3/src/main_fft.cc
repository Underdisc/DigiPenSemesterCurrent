////////////////////////////////////////////////////////////////////////////////
// File:        main_fft.cc
// Class:       MAT320
// Asssignment: Project 2
// Author:      Connor Deakin
// Email:       connor.deakin@digipen.edu
// Date:        2018-10-12
////////////////////////////////////////////////////////////////////////////////

#include <iostream>

#include "Complex.h"

// DFT & InverseDFT ////////////////////////////////////////////////////////////

#define PI  3.14159265358979323846
#define TAU 6.28318530717958647692

Complex FFTRecursive(unsigned k,
                     const std::vector<Complex> & v_input,
                     unsigned num_samples)
{
    if(num_samples == 1)
    {
        return v_input[0];
    }

    unsigned M = num_samples / 2;
    std::vector<Complex> even_input(M);
    std::vector<Complex> odd_input(M);
    for(unsigned m = 0; m < M; ++m)
    {
        even_input[m] = v_input[2 * m];
        odd_input[m] = v_input[2 *m + 1];
    }

    Complex e = FFTRecursive(k, even_input, M);
    Complex o = FFTRecursive(k, odd_input, M);
    Complex unity_root;
    unity_root.Polar(1.0, -PI * k / M);
    return e + unity_root * o;
}

void FFT(std::vector<Complex> * v_output,
         const std::vector<Complex> & v_input,
         unsigned num_samples)
{
    unsigned sample_check = num_samples;
    while(sample_check % 2 == 0)
    {
        sample_check /= 2;
    }
    if(sample_check != 1)
    {
        std::cout << "Error: N must be a power of 2 to perform the fft."
                  << std::endl;
        return;
    }

    for(unsigned k = 0; k < num_samples; ++k)
    {
        Complex output = FFTRecursive(k, v_input, num_samples);
        v_output->push_back(output);
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
    FFT(&v_output, v_input, samples);

    for(const Complex & c: v_output)
    {
        std::cout << c << std::endl;
    }
}
