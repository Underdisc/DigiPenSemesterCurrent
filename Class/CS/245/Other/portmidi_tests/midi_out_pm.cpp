// midi_out_pm.cpp
// -- sending MIDI messages, PortMidi version
// cs245 1/18
//
// usage:
//   midi_out_pm [<device> <pitch> <patch>]
// where:
//   <device> -- output device number
//   <pitch>  -- the pitch number to play (0..127)
//   <patch>  -- tne patch number to play
// notes:
//   - If no parameters are present, the program outputs a list of
//     MIDI devices.
//   - Otherwise, a note is sent to the specified device.
//
// To complie from VS command prompt:
//   cl /EHsc midi_out_pm.cpp portmidi.lib
//
// From Linux command line:
//   g++ midi_out_pm.cpp -lportmidi

#include <iostream>
#include <cstdlib>
#include <portmidi.h>
using namespace std;


union MidiMessage {
  long message;
  unsigned char byte[4];
};


int main(int argc, char *argv[]) {
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

  if (argc != 4)
    return 0;

  int device = atoi(argv[1]),
      pitch  = atoi(argv[2]),
      patch  = atoi(argv[3]);

  Pm_Initialize();
  PmStream *stream;
  PmError value = Pm_OpenOutput(&stream,device,0,64,0,0,0);
  if (value != pmNoError) {
    Pm_Terminate();
    cout << "failed to open MIDI output device" << endl;
    return -1;
  }

  MidiMessage msg;
  msg.byte[0] = 0xc0;
  msg.byte[1] = patch & 0x7f;
  Pm_WriteShort(stream,0,msg.message);

  msg.byte[0] = 0x90;
  msg.byte[1] = pitch & 0x7f;
  msg.byte[2] = 127;
  Pm_WriteShort(stream,0,msg.message);

  cin.get();

  msg.byte[0] = 0x90;
  msg.byte[1] = pitch & 0x7f;
  msg.byte[2] = 0;
  Pm_WriteShort(stream,0,msg.message);

  Pm_Close(stream);
  Pm_Terminate();

  return 0;
}

