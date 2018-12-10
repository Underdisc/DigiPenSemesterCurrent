// EqualizerDemo.cpp
// cs246 10/18
//
// usage:
//  EqualizerDemo [<file>]
// where:
//   <file> -- (optional) name of a WAVE file
// note:
//   if no file is specified, white noise is used

#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <sstream>
#include <string>
#include <cmath>
#include <cstdlib>
#include <portaudio.h>
#include "Control.h"
#include "Equalizer.h"
using namespace std;


class EqualizerDemo : public Control {
  public:
    EqualizerDemo(int,float,short*,unsigned,bool);
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
    Equalizer equalizerL, equalizerR;
    vector<string> labels;
};


EqualizerDemo::EqualizerDemo(int n, float R, short *s,
                             unsigned cnt, bool st)
    : Control(n,"Equalizer Demo"),
      samples(s),
      count(cnt),
      index(0),
      stereo(st),
      rate(R),
      equalizerL(n,R),
      equalizerR(n,R) {
  for (int i=0; i < n; ++i) {
    float f_low = 20*pow(1000,float(i)/n),
          f_high = 20*pow(1000,float(i+1)/n);
    stringstream ss;
    ss << fixed << "Band " << (i+1) << " [";
    if (f_low < 1000)
      ss << setprecision(0) << f_low;
    else
      ss << setprecision(1) << (f_low/1000) << "k";
    ss << "-";
    if (f_high < 1000)
      ss << setprecision(0) << f_high;
    else
      ss << setprecision(1)<< (f_high/1000) << "k";
    ss << " Hz] : ";
    labels.push_back(ss.str());
    ss << 0 << " dB";
    setLabel(i,ss.str().c_str());
    setRange(i,0,1000);
    setValue(i,500);
  }
}


int EqualizerDemo::OnWrite(const void *vin, void *vout,
                           unsigned long frames,
                           const PaStreamCallbackTimeInfo *tinfo,
                           PaStreamCallbackFlags flags, void *user) {
  const float IMAX = 1.0f/float(1<<15);

  EqualizerDemo& demo = *reinterpret_cast<EqualizerDemo*>(user);
  float *out = reinterpret_cast<float*>(vout);

  if (demo.stereo) {
    for (unsigned long i=0; i < frames; ++i) {
      float L = IMAX * demo.samples[2*demo.index],
            R = IMAX * demo.samples[2*demo.index+1];
      out[2*i] = demo.equalizerL(L);
      out[2*i+1] = demo.equalizerR(R);
      demo.index = (demo.index + 1) % demo.count;
    }
  }

  else {
    for (unsigned long i=0; i < frames; ++i) {
      float x = IMAX * demo.samples[demo.index];
      out[i] = demo.equalizerL(x);
      demo.index = (demo.index + 1) % demo.count;
    }
  }

  return paContinue;
}


void EqualizerDemo::valueChanged(unsigned n, int v) {
  float dB = 0.036f*v - 18,
        gain = pow(10,dB/20.0f);
  equalizerL.setGain(n,gain);
  equalizerR.setGain(n,gain);
  stringstream ss;
  ss << labels[n] << fixed << setprecision(1) << dB << " dB";
  setLabel(n,ss.str().c_str());
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

  Equalizer *eq_temp = new Equalizer(10,rate);
  int band_count = eq_temp->bandCount();
  delete eq_temp;
  EqualizerDemo eq(band_count,rate,samples,count,channels==2);

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
                EqualizerDemo::OnWrite,&eq);
  Pa_StartStream(stream);
  eq.show();

  cin.get();

  eq.show(false);
  Pa_StopStream(stream);
  Pa_CloseStream(stream);
  Pa_Terminate();

  delete[] samples;
  return 0;
}

