#ifndef CS245_SIMPLESYNTH_H
#define CS245_SIMPLESYNTH_H

#include "MidiIn.h"
#include <iostream>

using namespace std;

struct SimpleSynth : MidiIn {
  SimpleSynth(int n, int r) : MidiIn(n) { }
  ~SimpleSynth() { }
  float operator()(void) { return 0; }
  void onNoteOn(int channel, int note, int velocity) {
    cout <<  "note on: " << channel << ' '
         << note << ' ' << velocity << endl;
  }
  void onNoteOff(int channel, int note) {
    cout << "note off: " << channel << ' ' << note << endl;
  }
  void onPitchWheelChange(int channel, float value) {
    cout << "pitch wheel: " << channel << ' ' << value << endl;
  }
  void onVolumeChange(int channel, int level) {
    cout << "volume: " << channel << ' ' << level << endl;
  }
  void onModulationWheelChange(int channel, int value) {
    cout << "mod wheel: " << channel << ' ' << value << endl;
  }
  void onControlChange(int channel, int number, int value) {
    cout << "control: " << channel << ' '
         << number << ' ' << value << endl;
  }
};

/*
class SimpleSynth : private MidiIn
{
public:
  SimpleSynth(int devno, int R);
  ~SimpleSynth();
  float operator()(void);
  void onNoteOn(int channel, int note, int velocity);
  void onNoteOff(int channel, int note);
  void onPitchWheelChange(int channel, float value);
  void onVolumeChange(int channel, int level);
  void onModulationWheelChange(int channel, int value);
  void onControlChange(int channel, int number, int value);
};
*/
#endif
