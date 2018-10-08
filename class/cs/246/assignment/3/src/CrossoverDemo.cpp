// CrossoverDemo.cpp
// -- bass/treble control, variable crossover point
// jsh 9/18
//
// usage:
//   CrossoverDemo [<file>]
// where:
//   <file> -- (optional) name of a WAVE file
// note:
//   if no file is specified, white noise is used
//
// to compile:
//   cl /EHsc CrossoverDemo.cpp LowPass.cpp HighPass.cpp\
//      Control.cpp portaudio_x86.lib user32.lib

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cmath>
#include <cstdlib>
#include <portaudio.h>
#include "Control.h"
#include "LowPass.h"
#include "HighPass.h"
using namespace std;


class Crossover : public Control {
  public:
    Crossover(float R, short *s, unsigned n, bool st);
    static int OnWrite(const void*,void*,unsigned long,
                       const PaStreamCallbackTimeInfo*,
                       PaStreamCallbackFlags,void*);
  private:
    void valueChanged(unsigned,int);
    short *samples;
    unsigned count,
             index;
    bool stereo;
    float rate,
          gainL, gainH;
    LowPass lpL, lpR;
    HighPass hpL, hpR;
};


Crossover::Crossover(float R, short *s, unsigned n, bool st)
    : Control(3,"Crossover Demo"),
      samples(s),
      count(n),
      index(0),
      stereo(st),
      rate(R),
      gainL(1), gainH(1),
      lpL(200,R), lpR(200,R),
      hpL(200,R), hpR(200,R) {
  // 0: crossover point
  setLabel(0,"Crossover freqeuncy: 200 Hz");
  setRange(0,0,1000); // 20-2000 Hz (log scale)
  setValue(0,500);    // 200 Hz
  // 1: bass gain
  setLabel(1,"Bass gain: 0 dB");
  setRange(1,0,1000); // -24 - 24 dB
  setValue(1,500);    // 0 dB
  // 2: treble gain
  setLabel(2,"Treble gain: 0 dB");
  setRange(2,0,1000); // -24 - 24 dB
  setValue(2,500);    // 0 dB
}


void Crossover::valueChanged(unsigned index, int value) {
  const float TWOPI = 8.0f*atan(1.0f);
  stringstream ss;
  switch (index) {

    case 0: // crossover freq
      { float f = 20 * pow(10,0.002f*value);
        lpL.setFrequency(f);
        lpR.setFrequency(f);
        hpL.setFrequency(f);
        hpR.setFrequency(f);
        ss << "Crossover frequency: " << fixed
           << setprecision(1) << f << " Hz";
      }
      break;

    case 1: // bass gain
      { float dB = -24+0.048f*value;
        gainL = pow(10,dB/20.0f);
        ss << "Bass gain: " << fixed << setprecision(1)
           << dB << " dB";
      }
      break;

    case 2: // treble gain
      { float dB = -24+0.048f*value;
        gainH = pow(10,dB/20.0f);
        ss << "Treble gain: " << fixed << setprecision(1)
           << dB << " dB";
      }
      break;
   }
   setLabel(index,ss.str().c_str());
}


int Crossover::OnWrite(const void *vin, void *vout, unsigned long frames,
                       const PaStreamCallbackTimeInfo *tinfo,
                       PaStreamCallbackFlags flags, void *user) {
  const float IMAX = 1.0f/float(1<<15);

  Crossover& cr = *reinterpret_cast<Crossover*>(user);
  float *out = reinterpret_cast<float*>(vout);
              
  if (cr.stereo) {
    for (unsigned long i=0; i < frames; ++i) {
      float L = IMAX * cr.samples[2*cr.index],
            R = IMAX * cr.samples[2*cr.index+1];
      out[2*i] = cr.gainL * cr.lpL(L) + cr.gainH * cr.hpL(L);
      out[2*i+1] = cr.gainL * cr.lpR(R) + cr.gainH * cr.hpR(R);
      cr.index = (cr.index + 1) % cr.count;
    }
  }

  else {
    for (unsigned long i=0; i < frames; ++i) {
      float x = IMAX * cr.samples[cr.index]; 
      out[i] = cr.gainL * cr.lpL(x) + cr.gainH * cr.hpL(x);
      cr.index = (cr.index + 1) % cr.count;
    }
  }

  return paContinue;
}


int main(int argc, char *argv[]) {
  const int DEF_RATE = 44100;
  const float MAX = float((1<<15) - 1);

  if (argc != 1 && argc != 2)
    return 0;

  short *samples;
  unsigned channels,
           rate,
           count,
           size;

  // read in input file
  if (argc == 2) {
    fstream in(argv[1],ios_base::binary|ios_base::in);
    char header[44];
    in.read(header,44);
    channels = *reinterpret_cast<unsigned short*>(header+22),
    rate = *reinterpret_cast<unsigned*>(header+24),
    size = *reinterpret_cast<unsigned*>(header+40),
    count = size/(2*channels);
    samples = new short[channels*count];
    in.read(reinterpret_cast<char*>(samples),size);
    if (!in) {
      cout << "bad file: " << argv[1] << endl;
      return 0;
    }
  }

  // otherwise, generate white noise
  else {
    channels = 1;
    rate = DEF_RATE;
    count = rate;
    size = 2*count;
    samples = new short[count];
    for (int i=0; i < count; ++i)
      samples[i] = MAX * (1 - 2*float(rand())/float(RAND_MAX));
  }

  Crossover crossover(rate,samples,count,channels==2);

  Pa_Initialize();
  PaStreamParameters input_params;
  input_params.device = Pa_GetDefaultOutputDevice();
  input_params.channelCount = channels;
  input_params.sampleFormat = paFloat32;
  input_params.suggestedLatency = 2*Pa_GetDeviceInfo(input_params.device)
                                    ->defaultLowOutputLatency;
  input_params.hostApiSpecificStreamInfo = 0;

  PaStream *stream;
  Pa_OpenStream(&stream,0,&input_params,rate,0,0,
                Crossover::OnWrite,&crossover);
  Pa_StartStream(stream);
  crossover.show();

  cin.get();

  crossover.show(false);
  Pa_StopStream(stream);
  Pa_CloseStream(stream);
  Pa_Terminate();

  delete[] samples;
  return 0;
}

