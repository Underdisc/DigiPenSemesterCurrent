// BandPassTest.cpp
// -- test of band pass filter
// cs246 10/17

#include <algorithm>
#include <fstream>
#include "BandPass.h"
using namespace std;


class Frand {
  public:
    Frand(void)  : value(1031) { }
    float operator()(float a=0, float b=1);
  private:
    int value;
};


float Frand::operator()(float a, float b) {
  const int A = 2153,
            B = 4889,
            C = 7919;
  float x = a + (b-a)*float(value)/float(C);
  value = (A*value + B)%C;
  return x;
}


int main(void) {
  const unsigned RATE = 44100;
  const float MAX = float((1<<15)-1);
  Frand pseudorand;

  // pseudo-random noise
  unsigned count = 5*RATE;
  short *samples = new short[count];
  for (unsigned i=0; i < count; ++i)
    samples[i] = MAX * pseudorand(-1,1);

  // 1st second: fc = 200 Hz, Q = 2
  BandPass bp1(200,2,RATE);
  for (unsigned i=0; i < RATE; ++i) {
    float y = bp1(samples[i]);
    samples[i] = short(y);
  }

  // 2nd second: fc = 1000 Hz, Q = 1
  BandPass bp2(1000,1,RATE);
  for (unsigned i=0; i < RATE; ++i) {
    float y = bp2(samples[RATE+i]);
    samples[RATE+i] = short(y);
  }

  // 3rd second: fc = 4000 Hz, Q = 0.5
  BandPass bp3(4000,0.5f,RATE);
  for (unsigned i=0; i < RATE; ++i) {
    float y = bp3(samples[2*RATE+i]);
    samples[2*RATE+i] = short(y);
  }

  // 4th second: sweep of central frequency 0-10000 Hz, Q=3
  BandPass bp4(0,3,RATE);
  float factor = 10000.0f/RATE;
  for (unsigned i=0; i < RATE; ++i) {
    bp4.setFrequency(i*factor);
    float y = bp4(samples[3*RATE+i]);
    samples[3*RATE+i] = short(y);
  }

  // 5th second: fc=500 Hz, sweep of Q from 0.1 to 10
  BandPass bp5(500,0.1,RATE);
  factor = (10-0.1f)/RATE;
  for (unsigned i=0; i < RATE; ++i) {
    bp5.setQuality(0.1f+i*factor);
    float y = bp5(samples[4*RATE+i]);
    samples[4*RATE+i] = short(y);
  }


  // write output file
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
             36 + 2*count,
             {'W','A','V','E'},
             {'f','m','t',' '},
             16,1,1,RATE,2*RATE,2,16,
             {'d','a','t','a'},
             2*count
           };

  fstream out("BandPassTest.wav",ios_base::binary|ios_base::out);
  out.write(reinterpret_cast<char*>(&header),44);
  out.write(reinterpret_cast<char*>(samples),2*count);

  delete[] samples;
  return 0;
}

