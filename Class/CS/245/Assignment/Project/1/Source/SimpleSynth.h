

#ifndef CS245_SIMPLESYNTH_H
#define CS245_SIMPLESYNTH_H

class SimpleSynth : private MidiIn {
public:
  SimpleSynth(int devno, int R);
  ~SimpleSynth(void);
  float operator()(void);
};

#endif
