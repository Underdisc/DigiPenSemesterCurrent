// Echo.h
// -- echo filter
// cs246 9/18

#ifndef CS246_ECHO_H
#define CS246_ECHO_H

#include "Filter.h"
#include "RingBuffer.h"


class Echo : public Filter {
  public:
    Echo(float Tmax=1, float R=44100.0f);
    void setDelay(float t);
    void setMix(float a);
    void setFeedback(float b);
    float operator()(float x);
  private:
    int max_samples;
    RingBuffer xvalues, yvalues;
    float rate, mix, feedback, offset;
};


#endif

