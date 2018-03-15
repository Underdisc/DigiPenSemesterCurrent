#ifndef CS245_SIMPLESYNTH_H
#define CS245_SIMPLESYNTH_H

#include <vector>
#include "MidiIn.h"

#define NUM_CHANNELS 16

float SinWave(int R, float f);

struct Note
{
  Note(int midi_index, int midi_velocity);
  float CalculateSample(float time);
  float m_frequency;
  float m_gain;
  int m_id;
};

struct Channel
{
  void AddNote(int midi_index, int midi_velocity);
  void RemoveNote(int midi_index);
  float CalculateSample(float time);
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

  int m_sample_rate;
  int m_current_sample;

  Channel m_channels[NUM_CHANNELS];

};

#endif
