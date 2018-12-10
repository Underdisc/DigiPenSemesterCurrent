// LowPassResGainTest.cpp
// cs246 10/17

#include <fstream>
#include <algorithm>
#include <cmath>
#include "FilterGain.h"
#include "LowPassRes.h"
using namespace std;


int main(void) {
  const float MAX = float((1<<15)-1),
              RATE = 44100.0f,
              MINDB = -48.0f,
              MAXDB = 48.0f;
  const int COUNT = 1000;

  // compute log(f) vs log(gain) plot
  LowPassRes lp(500,0.95f,RATE);
  short *samples = new short[2*COUNT];
  for (int i=0; i < COUNT; ++i) {
    float frequency = 20.0f  * pow(10,3*float(i)/float(COUNT-1)),
          gain = filterGain(lp,frequency,RATE),
          dB = 20.0f * log10(gain);
    dB = min(MAXDB,max(MINDB,dB));
    samples[2*i+0] = short(MAX + 2*MAX*(dB-MAXDB)/(MAXDB-MINDB));
    gain = lp.theoreticalGain(frequency);
    dB = min(MAXDB,max(MINDB,20.0f*log10(gain)));
    samples[2*i+1] = short(MAX + 2*MAX*(dB-MAXDB)/(MAXDB-MINDB));
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
             16,1,2,RATE,4*RATE,2,16,
             {'d','a','t','a'},
             4*COUNT
           };
  fstream out("LowPassResGainTest.wav",ios_base::binary|ios_base::out);
  out.write(reinterpret_cast<char*>(&header),44);
  out.write(reinterpret_cast<char*>(samples),4*COUNT);

  return 0;
}

