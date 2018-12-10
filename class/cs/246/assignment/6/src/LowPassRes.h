////////////////////////////////////////////////////////////////////////////////
// File:        LowPassRes.h
// Class:       CS246
// Asssignment: 6
// Author:      Connor Deakin
// Email:       connor.deakin@digipen.edu
// Date:        2018-12-06
////////////////////////////////////////////////////////////////////////////////

#ifndef CS246_LOWPASSRES_H
#define CS246_LOWPASSRES_H

#include "Filter.h"

class LowPassRes : public Filter {
  public:
    LowPassRes(float f=0, float r=0, float R=44100);
    void setFrequency(float f);
    void setResonance(float r);
    float operator()(float x);
    float theoreticalGain(float f);
  private:
    float irate, resonance, cutoff;
    double zeta;
    double a0, b1, b2, b3, b4,
           y1, y2, y3, y4;
    void reset(void);
};

#endif

