// HighPass.h
// -- (improved) 1st order HP filter
// cs246 9/18

#ifndef CS246_HIGHPASS_H
#define CS246_HIGHPASS_H

#include "Filter.h"


class HighPass : public Filter {
  public:
    HighPass(float freq=0.0f, float rate=44100.0f);
    void setFrequency(float freq);
    float operator()(float x);
  private:
    double irate,
           a0, b1,
           x1, y1;
};


#endif

