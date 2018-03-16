#ifndef CS245_SIMPLESYNTH_H
#define CS245_SIMPLESYNTH_H

#include <vector>
#include "MidiIn.h"

#define NUM_CHANNELS 16

class Envelope
{
  float m_attack_time;
  float m_decay_time;
  float m_sustain;
  float m_release_time;
};

//============================================================================//
// Waveform //
//============================================================================//

// prolly need another thing for envelope
// which would get calculated as part of the channel

class Waveform
{
public:
  Waveform();
  virtual float CalculateSample(float fractional_index) = 0;
  static float s_sample_rate;
};

class Sine : public Waveform
{
public:
  Sine(float frequency);
  float CalculateSample(float fractional_index);
  float m_omega;
};

//============================================================================//
// Note //
//============================================================================//

class Note
{
public:
  Note(int midi_index, int midi_velocity);
  float CalculateSample(float fractional_index);
  Waveform * m_waveform;
  float m_gain;
  int m_id;
};

//============================================================================//
// Channel //
//============================================================================//
class Channel
{
public:
  Channel::Channel();
  void AddNote(int midi_index, int midi_velocity);
  void RemoveNote(int midi_index);
  float CalculateSample();
  void PitchShift(float delta_cents);
  float m_gain;
private:
  std::vector<Note> m_notes;
  float m_f_index;
  float m_f_index_increment;
};

//============================================================================//
// SimpleSynth //
//============================================================================//

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

  // should probably initialize
  Channel m_channels[NUM_CHANNELS];

};

#endif
