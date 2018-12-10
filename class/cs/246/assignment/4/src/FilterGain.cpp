////////////////////////////////////////////////////////////////////////////////
// File:        FilterGain.cpp
// Class:       CS246
// Asssignment: 4
// Author:      Connor Deakin
// Email:       connor.deakin@digipen.edu
// Date:        2018-12-06
////////////////////////////////////////////////////////////////////////////////

#include <cmath>

#include "FilterGain.h"

#define PI  3.14159265358979323846f
#define TAU 6.28318530717958647692f

float filterGain(Filter& F, float f, float R)
{
    float i_rad = TAU * f / R;
    float c_rad = 0.0f;

    // Start up the filter so it has already actively began to process a
    // signal.
    const unsigned impulse_stop = 100;
    for(unsigned i = 0; i < impulse_stop; ++i)
    {
        F(sinf(c_rad));
        c_rad += i_rad;
    }

    // Find the maximum value in the filtered signal over the course of a
    // second.
    float max = 0.0f;
    for(unsigned i = 0; i < (unsigned)R; ++i)
    {
        float filtered_sample = F(sinf(c_rad));
        if(fabs(filtered_sample) > max)
        {
            max = fabs(filtered_sample);
        }
        c_rad += i_rad;
    }
    return max;
}