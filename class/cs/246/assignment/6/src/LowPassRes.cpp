////////////////////////////////////////////////////////////////////////////////
// File:        LowPassRes.cpp
// Class:       CS246
// Asssignment: 6
// Author:      Connor Deakin
// Email:       connor.deakin@digipen.edu
// Date:        2018-12-06
////////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <complex>

#include "LowPassRes.h"

#define PI 3.14159265358979323846
#define TAU 6.2831853071795864769

LowPassRes::LowPassRes(float f, float r, float R) : irate(R),
                                                    resonance(r),
                                                    cutoff(f)
{
    reset();
    y1 = 0.0f;
    y2 = 0.0f;
    y3 = 0.0f;
    y4 = 0.0f;
}

void LowPassRes::setFrequency(float f)
{
    cutoff = f;
    reset();
}

void LowPassRes::setResonance(float r)
{
    resonance = r;
    reset();
}

float LowPassRes::operator()(float x)
{
    float y = a0 * x + b1 * y1 + b2 * y2 + b3 * y3;
    y3 = y2;
    y2 = y1;
    y1 = y;
    return y;
}

float LowPassRes::theoreticalGain(float f)
{
    float w0 = TAU * cutoff;
    float w0_2 = w0 * w0;
    float w0_3 = w0_2 * w0;
    float wf = TAU * f;
    std::complex<float> i(0.0f, 1.0f);
    std::complex<float> s = wf * i;
    std::complex<float> fr_denom = s + w0;
    fr_denom = fr_denom * 
               (-1.0f * wf * wf - 2.0f * w0 * s * cosf(zeta) + w0_2);
    std::complex<float> fr = w0_3 / fr_denom;
    return abs(fr);
}

void LowPassRes::reset(void)
{
    double w = TAU * (double)cutoff;
    double tau = w / (double)irate;
    zeta = (PI / 6.0) * (4.0 - (double)resonance);
    double kappa = 1.0 - 2.0 * cos(zeta);
    double kappa_2 = kappa * kappa;
    double tau_2 = tau * tau;
    double tau_3 = tau * tau * tau;
    double gamma = 1.0 +  (kappa * tau) + (kappa * tau_2) + tau_3;
    a0 = (1.0 / gamma) * tau_3;
    b1 = (1.0 / gamma) * (3.0 + 2.0 * kappa * tau + kappa * tau_2);
    b2 = -(1.0 / gamma) * (3.0 + kappa * tau);
    b3 = (1.0 / gamma);
}
