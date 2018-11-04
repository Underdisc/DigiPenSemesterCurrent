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

void PrintComplexVector(const std::vector<Complex> & values)
{
    for(const Complex & value : values)
    {
        std::cout << value << std::endl;
    }

}

void BitReversal(unsigned N, std::vector<unsigned> * output)
{
    output->reserve(N);
    unsigned initial_mask = N >> 1;
    unsigned reversed = 0;
    for(int i = 0; i < N; ++i)
    {
        output->push_back(reversed);
        unsigned mask = initial_mask;
        while(reversed & mask)
        {
            reversed &= ~mask;
            mask >>= 1;
        }
        reversed |= mask;
    }
}

bool GetBit(unsigned bit, unsigned value)
{
    unsigned mask  = 1 << bit;
    unsigned masked = mask & value;
    return (masked > 0);
}

void FFTBitReversal(std::vector<Complex> * v_output,
                    const std::vector<Complex> & v_input,
                    unsigned samples)
{
    // Make sure that the number of samples in the input vector is a power of 2.
    unsigned sample_check = samples;
    unsigned branches = 0;
    while(sample_check % 2 == 0)
    {
        sample_check /= 2;
        ++branches;
    }
    if(sample_check != 1)
    {
        std::cout << "Error: N must be a power of 2 to perform the fft."
                  << std::endl;
        return;
    }
    
    // Find the all bit reversals.
    std::vector<unsigned> bit_reversals;
    BitReversal(samples, &bit_reversals);
    
    std::vector<std::vector<float>> k_values;
    std::vector<std::vector<float>> e_values;
    k_values.resize(samples);
    e_values.resize(samples);

    // Find all k and e values.
    unsigned prev_reversal = 0xFFFFFFFF;
    std::vector<float> init_k_vals(branches, 0.0f);
    std::vector<float> init_e_vals(branches, -1.0f); 
    std::vector<float> * prev_k_vals = &init_k_vals;
    std::vector<float> * prev_e_vals = &init_e_vals;
    for(unsigned i = 0; i < samples; ++i)
    {
        std::vector<float> & k_vals = k_values[i];
        std::vector<float> & e_vals = e_values[i];
        k_vals.reserve(branches);
        e_vals.reserve(branches);

        unsigned cur_reversal = bit_reversals[i];

        for(unsigned branch = 0; branch < branches; ++branch)
        {
            float prev_k = (*prev_k_vals)[branch];
            float prev_e = (*prev_e_vals)[branch];
            unsigned bit = (branches - 1) - branch;
            if(GetBit(bit, cur_reversal) == GetBit(bit, prev_reversal))
            {
                k_vals.push_back(prev_k + 1.0f);
                e_vals.push_back(prev_e);
            }
            else
            {
                k_vals.push_back(0.0f);
                e_vals.push_back(prev_e * -1.0f);
            }
        }
        prev_reversal = cur_reversal;
        prev_k_vals = &k_vals;
        prev_e_vals = &e_vals;
    }

    // Create two vectors for reading and writing complex values for each stage.
    std::vector<Complex> v_output_a;
    std::vector<Complex> v_output_b;
    v_output_a.resize(samples);
    v_output_b.resize(samples);
    std::vector<Complex> * write = &v_output_a;
    std::vector<Complex> * read = &v_output_b;

    // Switch the indicies of complex inputs using the values in the bit
    // reversed array.
    for(unsigned i = 0; i < samples; ++i)
    {
        unsigned input_index = bit_reversals[i];
        (*read)[i] = v_input[input_index];
    }

    // Since each stage uses some base angle for w, we can precompute these
    // values upfront and then use the k values to find the true angles of each
    // w complex number.
    std::vector<double> root_w_angles;
    root_w_angles.reserve(branches);
    double N = 1.0;
    for(unsigned stage = 0; stage < branches; ++stage)
    {
        N = N * 2.0;
        double angle = -TAU / N;
        root_w_angles.push_back(angle);
    }


    unsigned index_distance = 1;
    unsigned merge_size = 2;
    for(unsigned stage = 0; stage < branches; ++stage)
    {
        // Precompute all of the w values needed for this stage.
        std::vector<Complex> w_values;
        w_values.reserve(index_distance);
        for(unsigned i = 0; i < index_distance; ++i)
        {
            float k_val = k_values[i][stage];
            double angle = root_w_angles[stage] * k_val;
            Complex w_value;
            w_value.Polar(1.0, angle);
            w_values.push_back(w_value);
        }

        for(unsigned i = 0; i < samples; ++i)
        {
            float k_val = k_values[i][stage];
            float e_val = e_values[i][stage];
            Complex w = w_values[k_val];

            unsigned top_i = i;
            unsigned bot_i = i;

            if(i % merge_size >= merge_size / 2)
            {
                top_i -= index_distance;
            }
            else
            {
                bot_i += index_distance;
            }

            (*write)[i] = (*read)[top_i] + w * (*read)[bot_i] * e_val;
        }
        index_distance *= 2;
        merge_size *= 2;

        std::vector<Complex> * temp = write;
        write = read;
        read = temp;
    }

    (*v_output) = (*read);
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
    FFTBitReversal(&v_output, v_input, samples);

    for(const Complex & c: v_output)
    {
        std::cout << c << std::endl;
    }
}
