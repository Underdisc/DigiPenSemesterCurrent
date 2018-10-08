////////////////////////////////////////////////////////////////////////////////
// File:        LowPass.cpp
// Class:       CS246
// Asssignment: 3
// Author:      Connor Deakin
// Email:       connor.deakin@digipen.edu
// Date:        2018-10-01
////////////////////////////////////////////////////////////////////////////////

#include <cmath>

#include "LowPass.h"

#define PI 3.14159265358979323846264338327950288419716939937510582097f

LowPass::LowPass(float f, float R)
{
    irate = PI / R;
    setFrequency(f);
    x1 = 0.0;
    y1 = 0.0;
}

void LowPass::setFrequency(float f)
{
    double theta = std::tan((double)f * irate);
    a0 = theta / (1.0 + theta);
    b1 = (1.0 - theta) / (1.0 + theta);
}

float LowPass::operator()(float x)
{
    double output = a0 * ((double)x + x1) + b1 * (y1);
    x1 = x;
    y1 = output;
    return (float)output;
}

