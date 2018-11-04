////////////////////////////////////////////////////////////////////////////////
// File:        main_bitrev.cc
// Class:       MAT320
// Asssignment: Project 3
// Author:      Connor Deakin
// Email:       connor.deakin@digipen.edu
// Date:        2018-10-29
////////////////////////////////////////////////////////////////////////////////

#include <vector>
#include <iostream>

// BitReversal /////////////////////////////////////////////////////////////////

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

void PrintBits(unsigned value, int num_bits)
{
    for(int i = num_bits - 1; i >= 0; --i)
    {
        std::cout << ((value >> i) & 1);
    }
}

// Main ////////////////////////////////////////////////////////////////////////

int main(int argc, char ** argv)
{
    if(argc < 2)
    {
        std::cout << "Error: Incorrect number of arguments.\n"
                     "bitrev.exe N"
                  << std::endl;
        return 0;
    }

    unsigned N = (unsigned)atoi(argv[1]);
    unsigned N_check = N;
    unsigned num_bits = 0;
    while(N_check % 2 == 0)
    {
        N_check /= 2;
        num_bits += 1;
    }
    if(N_check != 1)
    {
        std::cout << "Error: N must be a power of 2 for this program."
                  << std::endl;
        return 0;
    }

    std::vector<unsigned> bit_reversals;
    BitReversal(N, &bit_reversals);
    for(int i = 0; i < bit_reversals.size(); ++i)
    {
        std::cout << i << ", ";
        PrintBits(i, num_bits);
        std::cout << ", " << bit_reversals[i] << ", ";
        PrintBits(bit_reversals[i], num_bits);
        std::cout << std::endl;
    }
}

