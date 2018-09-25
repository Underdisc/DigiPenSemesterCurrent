// EchoDemo.cpp
// -- real-time echo DSP demo
// cs246 9/18
//
// usage:
//   EchoDemo <file>
// where:
//   <file> -- WAV file
//
// to compile from the Visual Studio command prompt:
//    cl /EHsc EchoDemo.cpp Echo.cpp RingBuffer.cpp
//       Control.lib portaudio_x86.lib user32.lib

#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <cstdlib>
#include <portaudio.h>
#include "Control.h"
#include "Echo.h"
using namespace std;


struct EchoDemo : Control {
  Echo echo_left, echo_right;
  short *samples;
  unsigned count;
  float gain;
  bool stereo;
  unsigned index;
  EchoDemo(float,short*,unsigned,bool);
  void valueChanged(unsigned i, int v);
  static int OnWrite(const void*,void*,unsigned long,
                   const PaStreamCallbackTimeInfo*,
                   PaStreamCallbackFlags,void*);
};


EchoDemo::EchoDemo(float r, short* s, unsigned fr, bool st)
    : Control(4,"Echo Demo"),
      echo_left(1,r),
      echo_right(1,r),
      samples(s),
      count(fr),
      gain(1),
      stereo(st),
      index(0) {
  // 0: delay
  setLabel(0,"Delay: 100 ms");
  setRange(0,0,1000);
  setValue(0,100);
  echo_left.setDelay(0.1f);
  echo_right.setDelay(0.1f);
  // 1: mix
  setLabel(1,"Mix: 0.5");
  setRange(1,0,1000);
  setValue(1,500);
  echo_left.setMix(0.5f);
  echo_right.setMix(0.5f);
  // 2: feedback
  setLabel(2,"Feedback: 0");
  setRange(2,0,1000);
  setValue(2,0);
  echo_left.setFeedback(0);
  echo_right.setFeedback(0);
  // 3: gain
  setLabel(3,"Gain: 1");
  setRange(3,0,1000);
  setValue(3,500);
}


void EchoDemo::valueChanged(unsigned i, int v) {
  stringstream ss;
  switch (i) {
    case 0: // delay
      { float d = 0.001f * v;
        echo_left.setDelay(d);
        echo_right.setDelay(d);
        ss << "Delay: " << v << " ms";
      }
      break;
    case 1: // mix
      { float r = 0.001f * v;
        echo_left.setMix(r);
        echo_right.setMix(r);
        ss << "Mix: " << fixed << setprecision(2) << r;
      }
      break;
    case 2: // feedback
      { float f = 0.001f * v;
        echo_left.setFeedback(f);
        echo_right.setFeedback(f);
        ss << "Feedback: " << fixed << setprecision(2) << f;
      }
      break;
    case 3: // gain
      gain = 0.002f * v;
      ss << "Gain: " << fixed << setprecision(2) << gain;
  }
  setLabel(i,ss.str().c_str());
}


int OnWrite(const void *vin, void *vout, unsigned long frames,
                   const PaStreamCallbackTimeInfo *tinfo,
                   PaStreamCallbackFlags flags, void *user) {
  const float IMAX = 1.0f/(1<<15);

  EchoDemo& et = *reinterpret_cast<EchoDemo*>(user);
  float *out = reinterpret_cast<float*>(vout);

  if (et.stereo) {
    for (unsigned i=0; i < frames; ++i) {
      float g = et.gain * IMAX,
            L = g * et.samples[2*et.index],
            R = g * et.samples[2*et.index+1];
      L = et.echo_left(L);
      R = et.echo_right(R);
      out[2*i] = L;
      out[2*i+1] = R;
      et.index = (et.index+1) % et.count;
    }
  }

  else {
    for (unsigned i=0; i < frames; ++i) {
      float y = et.gain * IMAX * et.samples[et.index];
      y = et.echo_left(y);
      out[i] = y;
      et.index = (et.index+1) % et.count;
    }
  }

  return paContinue;
}


int main(int argc, char *argv[]) {
  if (argc != 2)
    return 0;

  // read in WAVE file
  fstream in(argv[1],ios_base::binary|ios_base::in);
  char header[44];
  in.read(header,44);
  unsigned channels = *reinterpret_cast<unsigned short*>(header+22),
           rate = *reinterpret_cast<unsigned*>(header+24),
           size = *reinterpret_cast<unsigned*>(header+40),
           count = size/(2*channels);
  short *samples = new short[channels*count];
  in.read(reinterpret_cast<char*>(samples),size);
  if (!in) {
    cout << "bad file: " << argv[1] << endl;
    return 0;
  }

  EchoDemo et(rate,samples,count,channels==2);

  Pa_Initialize();
  PaStreamParameters input_params;
  input_params.device = Pa_GetDefaultOutputDevice();
  input_params.channelCount = channels;
  input_params.sampleFormat = paFloat32;
  input_params.suggestedLatency = 2 * Pa_GetDeviceInfo(input_params.device)
                                        ->defaultLowOutputLatency;
  input_params.hostApiSpecificStreamInfo = 0;

  PaStream *stream;
  Pa_OpenStream(&stream,0,&input_params,rate,0,0,OnWrite,&et);
  Pa_StartStream(stream);
  et.show();

  cin.get();

  et.show(false);
  Pa_StopStream(stream);
  Pa_CloseStream(stream);
  Pa_Terminate();

  delete[] samples;
  return 0;
}
