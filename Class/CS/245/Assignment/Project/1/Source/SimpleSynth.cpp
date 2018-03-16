/*****************************************************************************/
/*!
\file SimpleSynth.cpp
\author Connor Deakin
\par E-mail: connortdeakin\@gmail.com
\par Project: Project 1
\date 16/03/2018
\brief
  Contains the implementation for the SimpleSynth.

*/
/*****************************************************************************/

#include <cmath>
#include <math.h>
#include "SimpleSynth.h"

#define PI  3.14159265f
#define TAU 6.283185307179586f
#define OCTAVE_CENTS 1200.0
#define OCTAVE_SEMITONES 12.0f
#define MAX_NOTES 4
#define MAGIC_GAIN_FACTOR 0.3f
#define PITCH_SHIFT_RANGE 200.0f
#define MAX_MIDI_VELOCITY 127.0f
#define DEFAULT_SAMPLE_RATE 44100.0f
#define BASE_FREQUENCY 440.0f


//============================================================================//
// Waveform //
//============================================================================//

// static initialization
float Waveform::s_sample_rate = DEFAULT_SAMPLE_RATE;

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
  float speed_up = std::pow(2.0f, delta_semtiones / OCTAVE_SEMITONES);
  float frequency = BASE_FREQUENCY * speed_up;
  Sine * sine_waveform = new Sine(frequency);
  m_waveform = reinterpret_cast<Waveform *>(sine_waveform);
  m_f_index = 0.0;
  m_gain = (float)midi_velocity /  MAX_MIDI_VELOCITY;
  m_id = midi_index;
}

void Note::IncrementFractionalIndex(double f_index_increment)
{
  m_f_index += f_index_increment;
}

float Note::CalculateSample()
{
  return m_waveform->CalculateSample((float)m_f_index) * m_gain;
}

//============================================================================//
// Channel //
//============================================================================//

Channel::Channel()
{
  m_gain = 1.0f;
  m_f_index_increment = 1.0;
}

void Channel::AddNote(int midi_index, int midi_velocity)
{
  if (m_notes.size() < MAX_NOTES)
    m_notes.push_back(Note(midi_index, midi_velocity));
}

void Channel::RemoveNote(int midi_index)
{
  // find note and remove
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
    sample += m_notes[i].CalculateSample();
    m_notes[i].IncrementFractionalIndex(m_f_index_increment);
  }
  sample *= MAGIC_GAIN_FACTOR * m_gain;
  return sample;
}

void Channel::PitchShift(double cents)
{
  double exponent = cents / OCTAVE_CENTS;
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
  value *= PITCH_SHIFT_RANGE;
  m_channels[channel].PitchShift(value);
}

void SimpleSynth::onVolumeChange(int channel, int level)
{
  m_channels[channel].m_gain = (float)level / 127.0f;
}

void SimpleSynth::onModulationWheelChange(int channel, int value)
{
}

void SimpleSynth::onControlChange(int channel, int number, int value)
{
}
