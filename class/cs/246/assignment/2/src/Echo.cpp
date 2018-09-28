////////////////////////////////////////////////////////////////////////////////
// File:        Echo.cpp
// Class:       CS246
// Asssignment: 2
// Author:      Connor Deakin
// Email:       connor.deakin@digipen.edu
// Date:        2018-09-28
////////////////////////////////////////////////////////////////////////////////

#include <cmath>

#include "Echo.h"

Echo::Echo(float Tmax, float R) : max_samples((int)std::floor(Tmax * R)),
                                  xvalues(max_samples),
                                  yvalues(max_samples),
                                  rate(R),
                                  mix(0.0f),
                                  feedback(0.0f),
                                  offset(0.0f)
{
}

void Echo::setDelay(float t)
{
    offset = t * rate;
}

void Echo::setMix(float a)
{
    mix = a;
}

void Echo::setFeedback(float b)
{
    feedback = b;
}

float Echo::operator()(float x)
{
    // Find the values of previous samples that will be used for the computation
    // of the next ouput.
    float xmk0, xmk1;
    int floor_offset = (int)std::floor(offset);
    xmk0 = xvalues.get(floor_offset);
    xmk1 = xvalues.get(floor_offset + 1);

    float ymk0, ymk1;
    ymk0 = yvalues.get(floor_offset);
    ymk1 = yvalues.get(floor_offset + 1);

    // Perform linear interpolation on the fetched sample values.
    float t = offset - (float)floor_offset;
    float xmk, ymk;
    xmk =  xmk0 + t * (xmk1 - xmk0);
    ymk =  ymk0 + t * (ymk1 - ymk0);

    // Find the output sample value and save the input and output values to the
    // ring buffers.
    float output = x + (mix - feedback) * xmk + feedback * ymk;
    xvalues.put(x);
    yvalues.put(output);
    
    return output;
}
