// HighPassTest.cpp
// -- test of high pass filter
// cs246 9/18

#include <algorithm>
#include <fstream>
#include "HighPass.h"
using namespace std;


class Frand {
  public:
    Frand(void);
    float operator()(float a=0, float b=1);
  private:
    int value;
};


Frand::Frand(void) : value(1031) {
}
    

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
  unsigned count = 4*RATE;
  short *samples = new short[count];
  for (unsigned i=0; i < count; ++i)
    samples[i] = MAX * pseudorand(-1,1);

  // 1st second: cutoff at 200 Hz
  HighPass hp1(200,RATE);
  for (unsigned i=0; i < RATE; ++i) {
    float y = hp1(samples[i]);
    samples[i] = short(y);
  }

  // 2nd second: cutoff at 2000 Hz
  HighPass hp2(2000,RATE);
  for (unsigned i=0; i < RATE; ++i) {
    float y = hp2(samples[RATE+i]);
    samples[RATE+i] = short(y);
  }

  // 3rd second: cutoff at 8000 Hz
  HighPass hp3(8000,RATE);
  for (unsigned i=0; i < RATE; ++i) {
    float y = hp3(samples[2*RATE+i]);
    samples[2*RATE+i] = short(y);
  }

  // 4th second: sweep of cutoff from 0 to 20000 Hz
  HighPass hp4(0,RATE);
  float factor = 20000.0f/RATE;
  for (unsigned i=0; i < RATE; ++i) {
    hp4.setFrequency(i*factor);
    float y = hp4(samples[3*RATE+i]);
    samples[3*RATE+i] = short(y);
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

  fstream out("HighPassTest.wav",ios_base::binary|ios_base::out);
  out.write(reinterpret_cast<char*>(&header),44);
  out.write(reinterpret_cast<char*>(samples),2*count);

  delete[] samples;
  return 0;
}

