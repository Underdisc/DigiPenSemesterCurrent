// SimpleSynthDriver.cpp
// -- driver for the SimpleSynth class
// cs245 2/18
//
// usage:
//   SimpleSynthDriver [<devno>]
// where:
//   <devno> -- (optional) is the device number to use for MIDI input.
//              If not specified, a list of device is displayed.
//
// To complie from the Visual Studio 2015 command prompt:
//   cl /EHsc SimpleSynthDriver.cpp SimpleSynth.cpp MidiIn.lib
//      portaudio_x86.lib portmidi.lib pthreadVC2.lib
//
// From the Linux command line:
//   g++ SimpleSynthDriver.cpp SimpleSynth.cpp MidiIn.cpp
//       -lportaudio -lportmidi -pthread

#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <portaudio.h>
#include "MidiIn.h"
#include "SimpleSynth.h"
using namespace std;


/////////////////////////////////////////////////////////////////
// PortAudio callback
/////////////////////////////////////////////////////////////////
int onWrite(const void *vin, void *vout, unsigned long frames,
            const PaStreamCallbackTimeInfo *tinfo,
            PaStreamCallbackFlags flags, void *user) {
  float *out = reinterpret_cast<float*>(vout);
  SimpleSynth& synth = *reinterpret_cast<SimpleSynth*>(user);

  for (unsigned long i=0; i < frames; ++i)
    out[i] = synth();

  return paContinue;
}


/////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////
int main(int argc, char *argv[]) {
  const int RATE = 44100;

  if (argc == 1) {
    cout << MidiIn::getDeviceInfo() << endl;
    cout << "Re-run the program with the specified device to use as input" << endl;
    return 0;
  }

  if (argc != 2) {
    return -1;
  }

  int devno = atoi(argv[1]);
  SimpleSynth *synth = 0;
  try {
    synth = new SimpleSynth(devno,RATE);
  }
  catch (exception &e) {
    cout << e.what() << endl;
    return -1;
  }
  catch (...) {
    cout << "failed to open device" << endl;
    return -1;
  }

  Pa_Initialize();
  PaStreamParameters params;
  params.device = Pa_GetDefaultOutputDevice();
  params.channelCount = 1;
  params.sampleFormat = paFloat32;
  params.suggestedLatency = min(0.01,Pa_GetDeviceInfo(params.device)
                                     ->defaultLowOutputLatency);
  params.hostApiSpecificStreamInfo = 0;
  PaStream *output_stream;
  Pa_OpenStream(&output_stream,0,&params,RATE,0,0,onWrite,synth);
  Pa_StartStream(output_stream);

  cin.get();

  Pa_StopStream(output_stream);
  Pa_CloseStream(output_stream);
  Pa_Terminate();
  delete synth;

  return 0;
}
