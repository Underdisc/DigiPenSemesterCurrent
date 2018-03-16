


#include "SimpleSynth.h"

#include <cmath>
#include <math.h>
#include <iostream>

#define MAGIC_GAIN_FACTOR 0.3f

#define PI  3.14159265f
#define TAU 6.283185307179586f

#define MAX_MIDI_VELOCITY 127.0f

int index = 0;

//============================================================================//
// Waveform //
//============================================================================//

// static initialization
float Waveform::s_sample_rate = 44100.0f;

Waveform::Waveform() : m_f_index(0.0f), m_f_index_increment(1.0f)
{}

void Waveform::IncrementSample()
{
  m_f_index += m_f_index_increment;
}

// Sine : Waveform //=====//

Sine::Sine(float frequency) :
  m_omega(TAU * frequency / s_sample_rate)
{}

float Sine::CalculateSample()
{
  return std::sin(m_omega * m_f_index);
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

float Note::CalculateSample()
{
  float sample = m_waveform->CalculateSample() * m_gain;
  m_waveform->IncrementSample();
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

float Channel::CalculateSample()
{
  float sample = 0.0f;
  size_t num_notes = m_notes.size();
  for(int i = 0; i < num_notes; ++i)
  {
    sample += m_notes[i].CalculateSample();
  }
  sample *= MAGIC_GAIN_FACTOR;
  return sample;
}


//============================================================================//
// SimpleSynth //
//============================================================================//
SimpleSynth::SimpleSynth(int devno, int R) : MidiIn(devno),
  m_current_sample(0)
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
