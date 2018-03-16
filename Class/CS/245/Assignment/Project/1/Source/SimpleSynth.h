/*****************************************************************************/
/*!
\file SimpleSynth.h
\author Connor Deakin
\par E-mail: connortdeakin\@gmail.com
\par Assignment: Project 1
\par Course: CS 245
\par Term: Spring 2018
\date 16/03/2018
\brief
  Contains the interface for the SimpleSynth.
*/
/*****************************************************************************/

#ifndef CS245_SIMPLESYNTH_H
#define CS245_SIMPLESYNTH_H

#include <vector>
#include "MidiIn.h"

#define NUM_CHANNELS 16

// not implemented
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
  // 2 * PI * f / R
  float m_omega;
};

//============================================================================//
// Note //
//============================================================================//

class Note
{
public:
  Note(int midi_index, int midi_velocity);
  void IncrementFractionalIndex(double f_index_increment);
  float CalculateSample();
  // The waveform used by a note
  Waveform * m_waveform;
  // Envelope will likely go here
  // The current fractional index of the note
  double m_f_index;
  float m_gain;
  // The midi index associated with the note
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
  void PitchShift(double delta_cents);
  float m_gain;
private:
  std::vector<Note> m_notes;
  // The fractional index notes will be incremented by
  double m_f_index_increment;
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
  // The midi channels that can play notes
  Channel m_channels[NUM_CHANNELS];
};

#endif
