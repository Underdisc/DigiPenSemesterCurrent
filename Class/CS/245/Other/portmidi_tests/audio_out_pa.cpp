// audio_out_pa.cpp
// -- dynamic audio output using PortAudio
// cs246 2/18
//
// To compile from Visual Studio 2015 command prompt:
//   cl /EHsc audio_out_pa.cpp portaudio_x86.lib
//
// From the Linux command line:
//   g++ audio_out_pa.cpp -lportaudio
//
// Note: hit the ENTER key to stop audio playback

#include <iostream>
#include <cmath>
#include <algorithm>
#include <portaudio.h>
#include <cstdlib>
using namespace std;

#define PI  3.14159265f
#define TAU 6.283185307179586f

/////////////////////////////////////////////////////////////////
// class for generating successive audio samples
/////////////////////////////////////////////////////////////////
class Warble {
  public:
    Warble(float frequency, int R);
    float operator()(void);
  private:
    float omega;
    int index;
};


Warble::Warble(float f, int R)
    : omega(TAU * f/R), index(0) {
}


float Warble::operator()(void) {
  float angle = sin(omega * index);
  ++index;
  return angle;
}


/////////////////////////////////////////////////////////////////
// callback function needed by PortAudio
/////////////////////////////////////////////////////////////////
int onWrite(const void *vin, void *vout, unsigned long frames,
            const PaStreamCallbackTimeInfo *tinfo,
            PaStreamCallbackFlags flags, void *user) {
  float *out = reinterpret_cast<float*>(vout);
  Warble *signal = reinterpret_cast<Warble*>(user);

  for (unsigned long i=0; i < frames; ++i) {
    out[2*i+0] = signal[0]();
    out[2*i+1] = signal[1]();
  }

  return paContinue;
}


/////////////////////////////////////////////////////////////////
int main(void) {
  const int RATE = 44100;

  Warble warble[2] = { Warble(220,RATE), Warble(222,RATE) };

  Pa_Initialize();

  PaStreamParameters params;
  params.device = Pa_GetDefaultOutputDevice();
  params.channelCount = 2;
  params.sampleFormat = paFloat32;
  params.suggestedLatency = std::min(0.01, Pa_GetDeviceInfo(params.device)
                            ->defaultHighOutputLatency);
  std::cout << params.suggestedLatency << std::endl;
  params.hostApiSpecificStreamInfo = 0;
  PaStream *stream;
  Pa_OpenStream(&stream,0,&params,RATE,0,0,onWrite,warble);

  Pa_StartStream(stream);
  cin.get();
  Pa_StopStream(stream);

  Pa_CloseStream(stream);
  Pa_Terminate();
  return 0;
}
