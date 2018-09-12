// midi_intercept_pm.cpp
// -- intercept and print MIDI messages, PortMidi version
// cs245 1/18
//
// usage:
//   midi_intercept_pm [<in>] [<out>]
// where:
//  <in>  -- (optional) is the input device
//  <out> -- (optional) is the output device
// note:
//   hit the enter key to stop the program
//
// To compile from the MSVC 2015 command prompt:
//   cl /EHsc midi_intercept_pm.cpp portmidi.lib pthreadVC2.lib
//
// From the Linux command line:
//   g++ midi_intercept_pm.cpp -lportaudio -pthread

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <pthread.h>
#include <portmidi.h>
using namespace std;


// data needed by message processing thread
struct Data {
  bool kill;
  PmStream *in_stream,
           *out_stream;
};


// message processing thread function:
void* processMidi(void *vp) {
  // get data initialized in 'main'
  Data &data = *reinterpret_cast<Data*>(vp);

  // fetch and process messages until kill flag is set
  while (!data.kill) {
    // is there a message?
    if (Pm_Poll(data.in_stream)) {
      // fetch and process message
      union {
        long message;
        unsigned char byte[4];
      } msg;
      PmEvent event;
      Pm_Read(data.in_stream,&event,1);
      msg.message = event.message;
      cout << hex << int(msg.byte[0]) << 'h'
           << dec << ' ' << int(msg.byte[1])
                  << ' ' << int(msg.byte[2]) << endl;
      if (data.out_stream != 0)
        Pm_WriteShort(data.out_stream,0,msg.message);
    }
  }

  return 0;
}


int main(int argc, char *argv[]) {

  // no command line arguments: print device info
  if (argc == 1) {
    Pm_Initialize();
    int n = Pm_CountDevices();
    for (int i=0; i < n; ++i) {
      const PmDeviceInfo *info = Pm_GetDeviceInfo(i);
      cout << i << ": " << info->name << endl;
    }
    Pm_Terminate();
    return 0;
  }

  Pm_Initialize();

  // open output device (if specified)
  PmStream *out_stream = 0;
  if (argc == 3) {
    int device = atoi(argv[2]);
    PmError result = Pm_OpenOutput(&out_stream,device,0,64,0,0,0);
    if (result != pmNoError)
      cout << "failed to open output device" << endl;
  }

  // open input device
  int device = atoi(argv[1]);
  PmStream *in_stream;
  PmError result = Pm_OpenInput(&in_stream,device,0,64,0,0);
  if (result != pmNoError) {
    cout << "failed to open input device" << endl;
    if (out_stream != 0) {
      Pm_Close(out_stream);
      Pm_Terminate();
    }
    return -1;
  }
  
  // read input until ...
  pthread_t thread;
  Data data = { false, in_stream, out_stream };
  pthread_create(&thread,0,processMidi,&data);
  cin.get();
  data.kill = true;
  pthread_join(thread,0);

  Pm_Close(in_stream);
  if (out_stream != 0)
    Pm_Close(out_stream);
  Pm_Terminate();

  return 0;
}
