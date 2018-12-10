// BandPass.h
// -- simple BP filter (LP/HP in series)
// jsh 10/17

#ifndef JSH_BANDPASS_H
#define JSH_BANDPASS_H

#include "Filter.h"


class BandPass : public Filter {
  public:
    BandPass(float f=0, float Q=1, float R=44100);
    void setFrequency(float f);
    void setQuality(float Q);
    float operator()(float x);
  private:
    void reset(void);
    double irate,
           frequency,
           quality,
           a0, b1, b2,
           x1, x2, y1, y2;
};


#endif

