


#include "SimpleSynth.h"

#include <cmath>
#include <math.h>
#include <iostream>

#define PI  3.14159265f
#define TAU 6.283185307179586f

#define MAX_MIDI_VELOCITY 127.0f

int index = 0;

//============================================================================//
// Waveform Functions //
//============================================================================//

float SinWave(int R, float f)
{
  // we are going to need to figure out the incremental calculations
  // cause time is screwing shit up me thinks


  float omega = (TAU * f / R);
  float sample = 0.3f *sin(omega * index);
  ++index;
  return sample;
}

//============================================================================//
// Note //
//============================================================================//

Note::Note(int midi_index, int midi_velocity)
{
  float delta_semtiones = (float)(midi_index - 69);
  float speed_up = std::pow(2.0f, delta_semtiones / 12.0f);
  m_frequency = 440.0f * speed_up;
  m_gain = (float)midi_velocity /  MAX_MIDI_VELOCITY;
  m_id = midi_index;
}

float Note::CalculateSample(float time)
{
  float sample = SinWave(time, m_frequency) * m_gain;
  return sample;
}

//============================================================================//
// Channel //
//============================================================================//

void Channel::AddNote(int midi_index, int midi_velocity)
{
  m_notes.push_back(Note(midi_index, midi_velocity));
}

void Channel::RemoveNote(int midi_index)
{
  // performing linear search since we won't need to worry about to many notes
  // at once
  std::vector<Note>::iterator it = m_notes.begin();
  std::vector<Note>::iterator it_e = m_notes.end();
  for(; it != it_e; ++it)
  {
    if(it->m_id == midi_index)
    {
      m_notes.erase(it);
      return;
    }
  }
}

float Channel::CalculateSample(float time)
{
  float sample = 0.0f;
  size_t num_notes = m_notes.size();
  for(int i = 0; i < num_notes; ++i)
  {
    sample += m_notes[i].CalculateSample(time);
  }
  // TODO: TAKE GAIN INTO ACCOUNT
  return sample;
}


//============================================================================//
// SimpleSynth //
//============================================================================//
SimpleSynth::SimpleSynth(int devno, int R) : MidiIn(devno), m_sample_rate(R),
  m_current_sample(0)
{
  // begin processing midi events
  start();
}

SimpleSynth::~SimpleSynth()
{}

float SimpleSynth::operator()(void)
{
  float current_time = (float)m_current_sample / (float)m_sample_rate;
  float sample = 0.0f;
  for(int i = 0; i < NUM_CHANNELS; ++i)
  {
    sample += m_channels[i].CalculateSample(current_time);
  }
  ++m_current_sample;

  // return calculated value

  return SinWave(m_sample_rate, 440.0f);
}


void SimpleSynth::onNoteOn(int channel, int note, int velocity)
{
  m_channels[channel].AddNote(note, velocity);
}

void SimpleSynth::onNoteOff(int channel, int note)
{
  m_channels[channel].RemoveNote(note);
}

void SimpleSynth::onPitchWheelChange(int channel, float value)
{
  std::cout << "works" << std::endl;
}

void SimpleSynth::onVolumeChange(int channel, int level)
{
  std::cout << "works" << std::endl;
}

void SimpleSynth::onModulationWheelChange(int channel, int value)
{
  std::cout << "works" << std::endl;
}

void SimpleSynth::onControlChange(int channel, int number, int value)
{
  std::cout << "works" << std::endl;
}
