// ADSRTest.cpp
// -- simple test of the ADSR envelope class
// cs245 2/16

#include <fstream>
#include <cmath>
#include "ADSR.h"
using namespace std;


int main(void) {
  const float TWOPI = 8.0f*atan(1.0f),
              MAX = float((1<<15)-1);
  unsigned rate = 44100;
  float dtime=1.0f/rate,
        freq = 220.0f;
  unsigned count1 = unsigned(rate),
           count2 = 2*count1,
           count = count1 + count2,
           size = 2*count;
  short *samples = new short[count];

  // first part: no sustain (release mode ony!)
  {
    ADSR adsr(0,3.2f,0.5f,2.1f);
    adsr.sustainOff();
    for (unsigned i=0; i < count1; ++i)
      samples[i] = short(adsr()*MAX*sin(TWOPI*freq*i*dtime));
  }

  // second part: with sustain
  {
    ADSR adsr(0.2f,3.2f,0.5f,1.85f);
    for (unsigned i=0; i < count1; ++i)
      samples[i+count1] = short(adsr()*MAX*sin(TWOPI*freq*i*dtime));
    adsr.sustainOff();
    for (unsigned i=count1; i < count2; ++i)
      samples[i+count1] = short(adsr()*MAX*sin(TWOPI*freq*i*dtime));
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
             36 + size,
             {'W','A','V','E'},
             {'f','m','t',' '},
             16,1,1,rate,2*rate,2,16,
             {'d','a','t','a'},
             size
           };

  fstream out("ADSRTest.wav",ios_base::binary|ios_base::out);
  out.write(reinterpret_cast<char*>(&header),44);
  out.write(reinterpret_cast<char*>(samples),size);

  return 0;
}

