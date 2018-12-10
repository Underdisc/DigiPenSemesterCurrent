// BandPassDemo.cpp
// -- BP filter demo
// jsh 9/18
//
// usage:
//   BandPassDemo [<file>]
// where:
//   <file> -- (optional) name of a WAVE file
// note:
//   if no file is specified, white noise is used
//
// to compile:
//   cl /EHsc BandPassDemo.cpp BandPass.cpp\
//      Control.cpp portaudio_x86.lib user32.lib

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cmath>
#include <cstdlib>
#include <portaudio.h>
#include "Control.h"
#include "BandPass.h"
using namespace std;


class BandPassDemo : public Control {
  public:
    BandPassDemo(float,short*,unsigned,bool);
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
    BandPass bpL[3], bpR[3];
    float gain;
    int filter_count;
};


BandPassDemo::BandPassDemo(float R, short *s, unsigned n, bool st)
    : Control(4,"Band Pass Filter Demo"),
      samples(s),
      count(n),
      index(0),
      stereo(st),
      rate(R),
      gain(1),
      filter_count(1) {
  for (int i=0; i < 3; ++i) {
    bpL[i] = BandPass(500,1,R);
    bpR[i] = BandPass(500,1,R);
  }

  // 0: frequency
  setLabel(0,"Frequency: 500 Hz");
  setRange(0,0,1000);
  setValue(0,500);
  // 1: Q-factor
  setLabel(1,"Q-factor: 1");
  setRange(1,0,1000);
  setValue(1,91);
  // 2: roll-off
  setLabel(2,"Roll-off: 6 dB/oct");
  setRange(2,0,3);
  setValue(2,1);
  // 3: gain
  setLabel(3,"Gain: 0 dB");
  setRange(3,0,1000);
  setValue(3,500);
}


void BandPassDemo::valueChanged(unsigned index, int value) {
  stringstream ss;
  switch (index) {
    case 0: // frequency
      {  float frequency = 5*pow(10,1+0.002f*value);
         for (int i=0; i < 3; ++i) {
           bpL[i].setFrequency(frequency);
           bpR[i].setFrequency(frequency);
         }
         ss << "Frequency: " << fixed << setprecision(1)
            << frequency << " Hz";
      }
      break;
    case 1: // Q-factor
      { float Q = 0.0095f*value + 0.5f;
        for (int i=0; i < 3; ++i) {
          bpL[i].setQuality(Q);
          bpR[i].setQuality(Q);
        }
        ss << "Q-factor: " << fixed << setprecision(2) << Q;
      }
      break;
    case 2: // roll-off
      { filter_count = value;
        int rolloff = 6 * filter_count;
        ss << "Roll-off: " << rolloff << " dB/oct";
      }
      break;
    case 3: // gain
      { float dB = 0.04f*value - 20;
        gain = pow(10,dB/20.0f);
        ss << "Gain: " << fixed << setprecision(1) << dB << " dB";
      }
      break;
  }
  setLabel(index,ss.str().c_str());

}


int BandPassDemo::OnWrite(const void *vin, void *vout, unsigned long frames,
                          const PaStreamCallbackTimeInfo *tinfo,
                          PaStreamCallbackFlags flags, void *user) {
  const float IMAX = 1.0f/float(1<<15);

  BandPassDemo& demo = *reinterpret_cast<BandPassDemo*>(user);
  float *out = reinterpret_cast<float*>(vout);
              
  if (demo.stereo) {
    for (unsigned long i=0; i < frames; ++i) {
      float L = IMAX * demo.samples[2*demo.index],
            R = IMAX * demo.samples[2*demo.index+1];
      for (int n=0; n < demo.filter_count; ++n) {
        L = demo.bpL[n](L);
        R = demo.bpR[n](R);
      }
      out[2*i] = demo.gain * L;
      out[2*i+1] = demo.gain * R;
      demo.index = (demo.index + 1) % demo.count;
    }
  }

  else {
    for (unsigned long i=0; i < frames; ++i) {
      float x = IMAX * demo.samples[demo.index];
      for (int n=0; n < demo.filter_count; ++n)
        x = demo.bpL[n](x);
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

  BandPassDemo bp_demo(rate,samples,count,channels==2);

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
                BandPassDemo::OnWrite,&bp_demo);
  Pa_StartStream(stream);
  bp_demo.show();

  cin.get();

  bp_demo.show(false);
  Pa_StopStream(stream);
  Pa_CloseStream(stream);
  Pa_Terminate();

  delete[] samples;
  return 0;
}

