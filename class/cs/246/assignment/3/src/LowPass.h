// LowPass.h
// -- (improved) 1st order LP filter
// cs246 9/18

#ifndef CS246_LOWPASS_H
#define CS246_LOWPASS_H

#include "Filter.h"


class LowPass : public Filter {
  public:
    LowPass(float f=0.0f, float R=44100.0f);
    void setFrequency(float f);
    float operator()(float x);
  private:
    double irate,
           a0, b1,
           x1, y1;
};


#endif

