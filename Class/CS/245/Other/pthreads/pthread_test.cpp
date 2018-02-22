// pthread_test.cpp
// -- simple of threading
// cs245 2/18
//
// usage:
//   pthread_test [<devno>]
// where:
//   <devno> -- MIDI output device number
// notes:
//   - If no arguments are present, a list of available devices is printed.
//   - Otherwise, the specified device is used until the ENTER key is hit.
//
// To compile using MSVC 2015 command prompt:
//    cl /EHsc pthread_test.cpp portmidi.lib pthreadVC2.lib
//
// From Linux command prompt:
//    g++ pthread_test.cpp -lportmidi -pthread


#include <iostream>
#include <cstdlib>
#include <ctime>
#include <pthread.h>
#include <portmidi.h>
using namespace std;


// data structure to share with thread function
struct Data {
  PmStream *midi_out;
  bool kill;
};


// union for MIDI messages
union MidiMessage {
  long as_long;
  unsigned char as_bytes[4];
};


// thread function
void *thread_func(void *vp) {
  Data &data = *reinterpret_cast<Data*>(vp);
  srand(unsigned(time(0)));

  int note_index = -1;
  clock_t clicks_elapsed = clock(),
          next_click = rand()%CLOCKS_PER_SEC;
  while (!data.kill) {
    clicks_elapsed = clock();

    if (clicks_elapsed >= next_click) {

      if (note_index < 0) {
        MidiMessage msg;
        msg.as_bytes[0] = 0xc1;
        msg.as_bytes[1] = rand()%128;
        Pm_WriteShort(data.midi_out,0,msg.as_long);
        note_index = 10 + rand()%100;
        msg.as_bytes[0] = 0x91;
        msg.as_bytes[1] = note_index;
        msg.as_bytes[2] = rand()%128;
        Pm_WriteShort(data.midi_out,0,msg.as_long);
        cout << "note on: " << note_index << " ("
             << int(msg.as_bytes[2]) << ")" << endl;
      }
    
      else {
        MidiMessage msg;
        msg.as_bytes[0] = 0x91;
        msg.as_bytes[1] = note_index;
        msg.as_bytes[2] = 0;
        Pm_WriteShort(data.midi_out,0,msg.as_long);
        note_index = -1;
        cout << "note off" << endl;
      }

      next_click += rand()%CLOCKS_PER_SEC;
    }
  }

  return 0;
}


/////////////////////////////////////////////////////////////////
int main(int argc, char *argv[]) {

  if (argc == 1) {
    // dump available MIDI devices
    Pm_Initialize();
    int device_count = Pm_CountDevices();
    for (int i=0; i < device_count; ++i) {
      const PmDeviceInfo *info = Pm_GetDeviceInfo(i);
      cout << i << ": " << info->name << endl;
    }
    Pm_Terminate();
  }

  if (argc != 2)
    return 0;

  // open output device
  Pm_Initialize();
  PmStream *stream;
  int device = atoi(argv[1]);
  PmError err = Pm_OpenOutput(&stream,device,0,64,0,0,0);
  if (err != pmNoError) {
    Pm_Terminate();
    cout << "Failed to open MIDI output device" << endl;
    return -1;
  }

  // create a thread
  pthread_t thread;
  Data data = { stream, false };
  pthread_create(&thread,0,thread_func,&data);

  // wait for ENTER key
  cin.get();

  // stop and wait for thread
  data.kill = true;
  pthread_join(thread,0);

  // clean up
  Pm_Close(stream);
  Pm_Terminate();
  return 0;
}

