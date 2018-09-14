// portaudio_output_demo.cpp
// -- using PortAudio for output
// cs246 9/18
//
// from a Visual Studio command prompt:
//   cl /EHsc portaudio_output_demo.cpp portaudio_x86.lib

#include <iostream>
#include <cmath>
#include <portaudio.h>
using namespace std;


struct Data {
  double argument,
         dargument;
};


float Spline(float x) {
  x = x - floor(x);
  return 16*(x-0.5) * (x<0.5 ? -x : x-1);
}


static int OnWrite(const void *vin, void *vout, unsigned long frames,
                   const PaStreamCallbackTimeInfo *tinfo,
                   PaStreamCallbackFlags flags, void *user) {
  float *out = reinterpret_cast<float*>(vout);
  Data *data = reinterpret_cast<Data*>(user);

  for (unsigned long i=0; i < frames; ++i) {
    out[i] = 0.75f * Spline(data->argument);
    data->argument += data->dargument;
  }

  return paContinue;
}


int main(void) {
  const double RATE = 44100;
  Data data = { 0, 220.0/RATE };

  Pa_Initialize();

  // output stream parameters
  PaStreamParameters output_params;
  output_params.device = Pa_GetDefaultOutputDevice();
  output_params.channelCount = 1;
  output_params.sampleFormat = paFloat32;
  output_params.suggestedLatency = Pa_GetDeviceInfo(output_params.device)
                                     ->defaultHighOutputLatency;
  output_params.hostApiSpecificStreamInfo = 0;

  // start output
  PaStream *stream;
  Pa_OpenStream(&stream,0,&output_params,RATE,0,paClipOff,OnWrite,&data);
  Pa_StartStream(stream);

  cin.get();

  // stop output
  Pa_StopStream(stream);
  Pa_CloseStream(stream);
  Pa_Terminate();

  return 0;
}

