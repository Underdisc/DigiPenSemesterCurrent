////////////////////////////////////////////////////////////////////////////////
// File:        Equalizer.cpp
// Class:       CS246
// Asssignment: 5
// Author:      Connor Deakin
// Email:       connor.deakin@digipen.edu
// Date:        2018-12-06
////////////////////////////////////////////////////////////////////////////////

#include <cmath>

#include "Equalizer.h"

Equalizer::Equalizer(int n, float R)
{
    // The max and min frequencies the equalizer will cover.
    float min_f = 20.0f;
    float max_f = 20000.0f;

    // The delta between a high pass filter frequency and a low pass filter
    // frequency.
    float log_delta = (log10(max_f) - log10(min_f)) / n;

    // The frequencies for the high and low pass filters.
    float f_high = 20.0f;
    float f_low;

    // Create all of the sub filters for the equalizer.
    _filters.reserve(n);
    for(unsigned i = 0; i < n; ++i)
    {
        // Find the central frequency and q factor.
        float expo = log10f(f_high) + log_delta;
        f_low = powf(10.0f, expo);
        float f_central = sqrt(f_low * f_high);
        float q = f_central / (f_low - f_high);

        // Create the sub filter.
        SubFilter sub_filter;
        sub_filter._filter = BandPass(f_central, q, R);
        sub_filter._gain = 1.0f;
        _filters.push_back(sub_filter);
        f_high = f_low;
    }
}

Equalizer::~Equalizer(void)
{}

int Equalizer::bandCount(void) const
{
    return _filters.size();
}

void Equalizer::setGain(int n, float g)
{
    _filters[n]._gain = g;
}

float Equalizer::operator()(float x)
{
    // Apply all sub filters in the equalizer.
    float y = 0.0f;
    for(SubFilter & sub_filter : _filters)
    {
        y += sub_filter._filter(x) * sub_filter._gain;
    }
    return y;
}
