////////////////////////////////////////////////////////////////////////////////
// File:        BandPass.cpp
// Class:       CS246
// Asssignment: 4
// Author:      Connor Deakin
// Email:       connor.deakin@digipen.edu
// Date:        2018-12-06
////////////////////////////////////////////////////////////////////////////////

#include <cmath>

#include "BandPass.h"

#define PI 3.14159265358979323846f

BandPass::BandPass(float f, float Q, float R) : frequency(f), 
                                                quality(Q),
                                                irate(R)
{
    reset();

    // Set the initial sample values.
    x1 = 0.0f;
    x2 = 0.0f;
    y1 = 0.0f;
    y2 = 0.0f;
}

void BandPass::setFrequency(float f)
{
    frequency = f;
    reset();
}

void BandPass::setQuality(float Q)
{
    quality = Q;
    reset();
}

float BandPass::operator()(float x)
{
    // Calculate the filtered sample.
    float y = (a0 * x) - (a0 * x2) + (b1 * y1) - (b2 * y2);

    // Set old sample values.
    x2 = x1;
    x1 = x;
    y2 = y1;
    y1 = y;

    return y;
}

void BandPass::reset()
{
    // Calculate frequencies for low pass and high pass filters.
    float f_delta = frequency / quality;
    float f_delta_2 = f_delta * f_delta;
    float f_central_2 = frequency * frequency;
    float f_high = (-f_delta + sqrtf(f_delta_2 + 4.0f * f_central_2)) / 2.0f;
    float f_low = f_high + f_delta;

    // Calculate low pass filter coefficients.
    float theta_low = tanf(PI * f_low / irate);
    float a_low = 1.0f / (1.0f + theta_low);
    float b_low = (1.0f - theta_low) / (1.0f + theta_low);

    // Calculate high pass filter coefficients.
    float theta_high = tanf(PI * f_high / irate);
    float a_high = 1.0f / (1.0f + theta_high);
    float b_high = (1.0f - theta_high) / (1.0f + theta_high);

    // Calculate band pass filter coefficients.
    a0 = a_high * (1.0f - a_low);
    b1 = b_high + b_low;
    b2 = b_high * b_low;
}
