#ifndef CS245_SIMPLESYNTH_H
#define CS245_SIMPLESYNTH_H

#include <vector>
#include "MidiIn.h"

#define NUM_CHANNELS 16

//============================================================================//
// Waveform //
//============================================================================//


class Waveform
{
public:
  Waveform();
  void IncrementSample();
  virtual float CalculateSample() = 0;
  float m_f_index;
  float m_f_index_increment;
  static float s_sample_rate;
};

class Sine : public Waveform
{
public:
  Sine(float frequency);
  float CalculateSample();
  float m_omega;
};

// maybe do a function callback for waveform type
// or inheritance (but new)
// or just put everything in Waveform
//

// well, every note is dependent on some waveform
// they are different only in frequency
// so we should have a waveform per note


struct Note
{
  Note(int midi_index, int midi_velocity);
  float CalculateSample();
  Waveform * m_waveform;
  float m_gain;
  int m_id;
};

struct Channel
{
  void AddNote(int midi_index, int midi_velocity);
  void RemoveNote(int midi_index);
  float CalculateSample();
  std::vector<Note> m_notes;
  float m_gain;
  float m_pitch_shift;
  bool m_balance;
};


class SimpleSynth : private MidiIn
{
public:
  SimpleSynth(int devno, int R);
  ~SimpleSynth();
  float operator()(void);
private:
  void onNoteOn(int channel, int note, int velocity);
  void onNoteOff(int channel, int note);
  void onPitchWheelChange(int channel, float value);
  void onVolumeChange(int channel, int level);
  void onModulationWheelChange(int channel, int value);
  void onControlChange(int channel, int number, int value);

  int m_current_sample;

  Channel m_channels[NUM_CHANNELS];

};

#endif
