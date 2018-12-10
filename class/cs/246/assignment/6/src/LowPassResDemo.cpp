// LowPassResDemo.cpp
// -- filter demo
// cs246 10/18
//
// usage:
//   LowPassResDemo [<file>]
// where:
//   <file> -- (optional) name of a WAVE file
// note:
//   if no file is specified, white noise is used
//
// to compile:
//   cl /EHsc LowPassResDemo.cpp LowPassRes.cpp\
//      Control.cpp portaudio_x86.lib user32.lib

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cmath>
#include <cstdlib>
#include <portaudio.h>
#include "Control.h"
#include "LowPassRes.h"
using namespace std;


class LowPassResDemo : public Control {
  public:
    LowPassResDemo(float,short*,unsigned,bool);
    static int OnWrite(const void*,void*,unsigned long,
                       const PaStreamCallbackTimeInfo*,
                       PaStreamCallbackFlags,void*);
  private:
    void valueChanged(unsigned,int);
    short *samples;
    unsigned count,
             index;
    bool stereo;
    float rate;
    LowPassRes lpL, lpR;
    float gain;
};


LowPassResDemo::LowPassResDemo(float R, short *s, unsigned n, bool st)
    : Control(3,"Resonant Low Pass Filter Demo"),
      samples(s),
      count(n),
      index(0),
      stereo(st),
      rate(R),
      lpL(500,0,R),
      lpR(500,0,R),
      gain(1) {

  // 0: frequency
  setRange(0,0,1000);   // range: 50Hz - 5000Hz
  setValue(0,500);      // default: 500Hz
  // 1: resonance
  setRange(1,0,1000);   // range: 0 - 1
  setValue(1,0);        // default: 0
  // 2: gain
  setRange(2,0,1000);   // range: -24dB - 24dB
  setValue(2,500);      // default: 0dB
}


void LowPassResDemo::valueChanged(unsigned index, int value) {
  stringstream ss;
  switch (index) {
    case 0: // frequency
      {  float frequency = 5*pow(10,1+0.002f*value);
         lpL.setFrequency(frequency);
         lpR.setFrequency(frequency);
         ss << "Frequency: " << fixed << setprecision(1)
            << frequency << " Hz";
      }
      break;
    case 1: // resonance
      { float r = 0.001f*value;
        lpL.setResonance(r);
        lpR.setResonance(r);
        ss << "Resonance: " << fixed << setprecision(3) << r;
      }
      break;
    case 2: // gain
      { float dB = 0.048f*value - 24;
        gain = pow(10,dB/20.0f);
        ss << "Gain: " << fixed << setprecision(1) << dB << " dB";
      }
      break;
  }
  setLabel(index,ss.str().c_str());

}


int LowPassResDemo::OnWrite(const void *vin, void *vout, unsigned long frames,
                            const PaStreamCallbackTimeInfo *tinfo,
                            PaStreamCallbackFlags flags, void *user) {
  const float IMAX = 1.0f/float(1<<15);

  LowPassResDemo& demo = *reinterpret_cast<LowPassResDemo*>(user);
  float *out = reinterpret_cast<float*>(vout);
              
  if (demo.stereo) {
    for (unsigned long i=0; i < frames; ++i) {
      float L = IMAX * demo.samples[2*demo.index],
            R = IMAX * demo.samples[2*demo.index+1];
      L = demo.lpL(L);
      R = demo.lpR(R);
      out[2*i] = demo.gain * L;
      out[2*i+1] = demo.gain * R;
      demo.index = (demo.index + 1) % demo.count;
    }
  }

  else {
    for (unsigned long i=0; i < frames; ++i) {
      float x = IMAX * demo.samples[demo.index];
      x = demo.lpL(x);
      out[i] = demo.gain * x;
      demo.index = (demo.index + 1) % demo.count;
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

  LowPassResDemo lp_demo(rate,samples,count,channels==2);

  Pa_Initialize();
  PaStreamParameters input_params;
  input_params.device = Pa_GetDefaultOutputDevice();
  input_params.channelCount = channels;
  input_params.sampleFormat = paFloat32;
  input_params.suggestedLatency = Pa_GetDeviceInfo(input_params.device)
                                  ->defaultLowOutputLatency;
  input_params.hostApiSpecificStreamInfo = 0;

  PaStream *stream;
  Pa_OpenStream(&stream,0,&input_params,rate,0,0,
                LowPassResDemo::OnWrite,&lp_demo);
  Pa_StartStream(stream);
  lp_demo.show();

  cin.get();

  lp_demo.show(false);
  Pa_StopStream(stream);
  Pa_CloseStream(stream);
  Pa_Terminate();

  delete[] samples;
  return 0;
}

