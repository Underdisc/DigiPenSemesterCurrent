// FilterGainTest.cpp
// -- produce a WAVE file that represents the frequency/gain plot
//    of a 2nd order LP filter.
// cs246 10/17

#include <fstream>
#include <cmath>
#include "FilterGain.h"
using namespace std;


// 2nd order Butterworth LP filter
class LP2 : public Filter {
  public:
    LP2(float f, float R);
    float operator()(float);
  private:
    float a0, b1, b2,
          y1, y2;
};


LP2::LP2(float f, float R)
    : y1(0),
      y2(0) {
  const float PI = 4.0f*atan(1.0f),
              RT2 = sqrt(2.0f);
  float s = 2*PI*f/R,
        norm = 1.0f/(1 + RT2*s + s*s);
  a0 = norm*s*s;
  b1 = norm*(2 + RT2*s);
  b2 = -norm;
}


float LP2::operator()(float x) {
  float y = a0*x + b1*y1 + b2*y2;
  y2 = y1;
  y1 = y;
  return y;
}



int main(void) {
  const float ILN10 = 1.0f/log(10.0f),
              MAX = float((1<<15)-1);
  const int RATE = 44100;
  const int COUNT = 1000;

  // compute log(f) vs log(gain) plot
  LP2 lp(664.1f,RATE);
  short *samples = new short[COUNT];
  for (int i=0; i < COUNT; ++i) {
    float frequency = 20.0f  * pow(0.5f*RATE/20.0f,float(i)/float(COUNT)),
          gain = filterGain(lp,frequency,RATE),
          dB = 20.0f * log(gain) * ILN10;
    dB = (dB < -96.0f) ? -96.0f : dB;
    samples[i] = short(MAX*(1+2.0f*dB/96.0f));
  }


  struct {
    char riff_chunk[4];
    unsigned chunk_size;
    char wave_fmt[4];
    char fmt_chunk[4];
    unsigned fmt_chunk_size;
    unsigned short audio_format;
    unsigned short number_of_channels;
    unsigned sampling_rate;
    unsigned bytes_per_second;
    unsigned short block_align;
    unsigned short bits_per_sample;
    char data_chunk[4];
    unsigned data_chunk_size;
  }
  header = { {'R','I','F','F'},
             36 + 4*COUNT,
             {'W','A','V','E'},
             {'f','m','t',' '},
             16,1,1,RATE,2*RATE,2,16,
             {'d','a','t','a'},
             2*COUNT
           };
  fstream out("FilterGainTest.wav",ios_base::binary|ios_base::out);
  out.write(reinterpret_cast<char*>(&header),44);
  out.write(reinterpret_cast<char*>(samples),2*COUNT);

  return 0;
}

