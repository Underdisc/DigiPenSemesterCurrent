


#include "SimpleSynth.h"

#include <cmath>
#include <math.h>
#include <iostream>

#define PI  3.14159265f
#define TAU 6.283185307179586f
#define MAGIC_GAIN_FACTOR 0.3f
#define MAX_MIDI_VELOCITY 127.0f
#define OCTAVE_CENTS 1200.0f

//============================================================================//
// Waveform //
//============================================================================//

// static initialization
float Waveform::s_sample_rate = 44100.0f;

Waveform::Waveform() {}

// Sine : Waveform //=========================================================//

Sine::Sine(float frequency) :
  m_omega(TAU * frequency / s_sample_rate)
{}

float Sine::CalculateSample(float fractional_index)
{
  return std::sin(m_omega * fractional_index);
}

//============================================================================//
// Note //
//============================================================================//

Note::Note(int midi_index, int midi_velocity)
{
  float delta_semtiones = (float)(midi_index - 69);
  float speed_up = std::pow(2.0f, delta_semtiones / 12.0f);
  float frequency = 440.0f * speed_up;
  Sine * sine_waveform = new Sine(frequency);
  m_waveform = reinterpret_cast<Waveform *>(sine_waveform);
  m_gain = (float)midi_velocity /  MAX_MIDI_VELOCITY;
  m_id = midi_index;
}

float Note::CalculateSample(float fractional_index)
{
  return m_waveform->CalculateSample(fractional_index) * m_gain;
}

//============================================================================//
// Channel //
//============================================================================//

Channel::Channel()
{
  m_gain = 1.0f;
  m_f_index = 0.0f;
  m_f_index_increment = 1.0f;
}

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

float Channel::CalculateSample()
{
  // calculuate sample value of all notes
  float sample = 0.0f;
  size_t num_notes = m_notes.size();
  for(int i = 0; i < num_notes; ++i)
  {
    sample += m_notes[i].CalculateSample(m_f_index);
  }
  sample *= MAGIC_GAIN_FACTOR * m_gain;
  // update fractional index for next sample
  m_f_index += m_f_index_increment;
  return sample;
}

void Channel::PitchShift(float cents)
{
  float exponent = cents / OCTAVE_CENTS;
  m_f_index_increment = std::pow(2.0f, exponent);
}


//============================================================================//
// SimpleSynth //
//============================================================================//
SimpleSynth::SimpleSynth(int devno, int R) : MidiIn(devno)
{
  // set waveform sample rate and begin processing midi events
  Waveform::s_sample_rate = (float)R;
  start();
}

SimpleSynth::~SimpleSynth()
{}

float SimpleSynth::operator()(void)
{
  float sample = 0.0f;
  for(int i = 0; i < NUM_CHANNELS; ++i)
  {
    sample += m_channels[i].CalculateSample();
  }
  return sample;
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
  value += 1.0f;
  value *= 200.0f;
  m_channels[channel].PitchShift(value);
}

void SimpleSynth::onVolumeChange(int channel, int level)
{
  m_channels[channel].m_gain = (float)level / 127.0f;
}

void SimpleSynth::onModulationWheelChange(int channel, int value)
{
  std::cout << "works" << std::endl;
}

void SimpleSynth::onControlChange(int channel, int number, int value)
{
  std::cout << "works" << std::endl;
}
