// Sine.h
// -- Computing the sine function using a look-up table
// cs245 3/18

#ifndef CS245_SINE_H
#define CS245_SINE_H


#include <vector>


class Sine {
  public:
    Sine(unsigned R);
    float operator()(float x);
  private:
    std::vector<float> sine_table;
    double scale;
};


#endif

