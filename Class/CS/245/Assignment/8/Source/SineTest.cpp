// SineTest.cpp
// -- test of Sine class
// cs245 3/15


#include <fstream>
#include <cmath>
#include "Sine.h"
using namespace std;


int main(void) {
  const unsigned RATE = 44100;
  const float MAX = float((1<<15)-1),
              PI = 4.0f*atan(1.0f);

  int count = 2*RATE;
  short *samples = new short[count];
  Sine sine(RATE);

  float factor = 2*PI*440/float(RATE),
        offset = -0.5*PI;
  for (int i=0; i < count; ++i)
    samples[i] = short(MAX * sine(factor*i+offset));

  unsigned size = 2*count;
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
             36 + size,
             {'W','A','V','E'},
             {'f','m','t',' '},
             16,1,1,RATE,2*RATE,2,16,
             {'d','a','t','a'},
             size
           };

  fstream out("SineTest.wav",ios_base::binary|ios_base::out);
  out.write(reinterpret_cast<char*>(&header),44);
  out.write(reinterpret_cast<char*>(samples),size);

  delete[] samples;
  return 0;
}

