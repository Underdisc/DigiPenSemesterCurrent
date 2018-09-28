// EchoTest.cpp
// cs246 9/17

#include <fstream>
#include <cmath>
#include "Echo.h"
using namespace std;


int main(void) {

  unsigned rate = 44100,
           count = 2*rate;
  float *fsamples = new float[count];
  float k = log(2.0f)/(0.1f*rate),
        twopi = 8.0f*atan(1.0f),
        omegac = 200.0f*twopi/rate,
        omegam = 137.0f*twopi/rate;
  for (unsigned i=0; i < count; ++i)
    fsamples[i] = 0.75f*exp(-k*i)*sin(omegac*i + 0.5f*sin(omegam*i));

  Echo echo(0.5f,(float)rate);
  echo.setDelay(0.2f);
  echo.setMix(0.5f);
  echo.setFeedback(0.5f);
  for (unsigned i=0; i < count; ++i)
    fsamples[i] = echo(fsamples[i]);

  short *samples = new short[count];
  float MAX = float((1<<15)-1);
  for (unsigned i=0; i < count; ++i)
    samples[i] = short(MAX*fsamples[i]);

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
             16,1,1,rate,2*rate,2,16,
             {'d','a','t','a'},
             2*count
           };

  fstream out("EchoTest.wav",ios_base::binary|ios_base::out);
  out.write(reinterpret_cast<char*>(&header),44);
  out.write(reinterpret_cast<char*>(samples),2*count);

  delete[] samples;
  delete[] fsamples;
  return 0;
}

