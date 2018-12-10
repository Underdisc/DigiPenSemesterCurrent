// VocoderDemo.cpp
// cs246 10/18
//
// usage:
//  VocoderDemo [<file>]
// where:
//   <file> -- (optional) name of a WAVE file
// note:
//   if no file is specified, white noise is used

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <portaudio.h>
#include "Control.h"
#include "Vocoder.h"
using namespace std;


const int CHANNEL_COUNTS[7] = { 8, 12, 16, 20, 24, 28, 32 };


/////////////////////////////////////////////////////////////////
class VocoderDemo : public Control {
  public:
    VocoderDemo(float R, short *samples, int frames, bool stereo);
    static int OnWrite(const void*,void*,unsigned long,
                       const PaStreamCallbackTimeInfo*,
                       PaStreamCallbackFlags,void*);
  private:
    void valueChanged(unsigned,int);
    short *samples;
    int frame_count,
        current_frame;
    bool is_stereo;
    Vocoder vocoderL[7], vocoderR[7];
    int actual_channels[7];
    int selected_vocoder;
    float gain;
};


VocoderDemo::VocoderDemo(float R, short *s, int frames, bool stereo)
    : Control(3,"Vocoder Demo"),
      samples(s),
      frame_count(frames),
      current_frame(0),
      is_stereo(stereo),
      selected_vocoder(3),
      gain(1) {
   for (int i=0; i < 7; ++i) {
     vocoderL[i] = Vocoder(CHANNEL_COUNTS[i],R);
     vocoderR[i] = Vocoder(CHANNEL_COUNTS[i],R);
     actual_channels[i] = vocoderL[i].bandCount();
   }
   stringstream ss;
   ss << "Channel count: " << CHANNEL_COUNTS[3];
   setLabel(0,ss.str().c_str());
   setRange(0,0,6);
   setValue(0,3);
   setLabel(1,"Pitch shift: 0 cents");
   setRange(1,0,800);
   setValue(1,400);
   setLabel(2,"Gain: 0 dB");
   setRange(2,0,1000);
   setValue(2,500);
   stringstream title;
   title << "Vocoder Demo: [" << actual_channels[3] << " channels]";
   setTitle(title.str().c_str());
}


int VocoderDemo::OnWrite(const void *vin, void *vout,
                         unsigned long frames,
                         const PaStreamCallbackTimeInfo *tinfo,
                         PaStreamCallbackFlags flags, void *user) {
  const float IMAX = 1.0f/float(1<<15);

  VocoderDemo& demo = *reinterpret_cast<VocoderDemo*>(user);
  float *out = reinterpret_cast<float*>(vout);

  if (demo.is_stereo) {
    for (unsigned long i=0; i < frames; ++i) {
      float L = IMAX * demo.samples[2*demo.current_frame],
            R = IMAX * demo.samples[2*demo.current_frame+1];
      out[2*i] = demo.gain * demo.vocoderL[demo.selected_vocoder](L);
      out[2*i+1] = demo.gain * demo.vocoderR[demo.selected_vocoder](R);
      demo.current_frame = (demo.current_frame + 1) % demo.frame_count;
    }
  }

  else {
    for (unsigned long i=0; i < frames; ++i) {
      float x = IMAX * demo.samples[demo.current_frame];
      out[i] = demo.gain * demo.vocoderL[demo.selected_vocoder](x);
      demo.current_frame = (demo.current_frame + 1) % demo.frame_count;
    }
  }

  return paContinue;
}


void VocoderDemo::valueChanged(unsigned n, int v) {
  stringstream ss;
  switch (n) {
    case 0:
      { selected_vocoder = v;
        stringstream title;
        title << "Vocoder Demo [" << actual_channels[v] << " channels]";
        setTitle(title.str().c_str());
        ss << "Channel count: " << CHANNEL_COUNTS[v];
      }
      break;
    case 1:
      { float p = 6*v - 2400;
        vocoderL[selected_vocoder].setOffset(p);
        vocoderR[selected_vocoder].setOffset(p);
        ss << "Pitch shift: " << int(p) << " cents";
      }
      break;
    case 2:
      { float dB = 0.048f*v - 24;
        gain = pow(10,dB/20.0f);
        ss << "Gain: " << fixed << setprecision(1) << dB << " dB";
      }
      break;
  }
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

  VocoderDemo vocoder(rate,samples,count,channels==2);

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
                VocoderDemo::OnWrite,&vocoder);
  Pa_StartStream(stream);
  vocoder.show();

  cin.get();

  vocoder.show(false);
  Pa_StopStream(stream);
  Pa_CloseStream(stream);
  Pa_Terminate();

  delete[] samples;
  return 0;
}

