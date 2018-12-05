/*****************************************************************************/
/*!
\file main_lowpass.cc
\author Connor Deakin
\par E-mail: connortdeakin\@digipen.edu
\par Project: 4
\par Course: MAT 320
\date 2018/11/26
\brief
    Contains a lowpass filter implementation. This will take an input wave file
    and output a wave file that has had n lowpass filters applied to it.

Usage:
./lowpass.exe a n filename
a - The lowpass filter factor.
n - The number of times the lowpass filter will be applied.
filename - The wave file containing the audio data that the lowpass filter will
    be applied to.
*/
/*****************************************************************************/

#include <iostream>
#include <cstdlib>
#include <math.h>
#include <queue>
#include <vector>

#include "AudioData.h"

#define TAU 6.28318530717958647692

float Random()
{
    float random = (float)rand() / (float)RAND_MAX;
    return random;
}

void PopulateWithNoise(AudioData & ad, unsigned start, unsigned samples)
{
    unsigned end = start + samples;
    for(unsigned cframe = start; cframe < end; ++cframe)
    {
        ad.sample(cframe) = Random();
    }
}

// PluckedStringFilter /////////////////////////////////////////////////////////
class PluckedStringFilter
{
public:
    PluckedStringFilter(float frequency, 
                        float sample_rate,
                        float R);
    void SetProperties(float frequency, 
                       float sample_rate,
                       float R);
    float Apply(float sample);
private:
    // The constants used by the filter.
    unsigned _L;
    float _RL;
    float _a;

    // Storage of previous values that are used by the filter.
    float _x1;
    float _x2;
    std::queue<float> _yL;
    float _yL1;
};

PluckedStringFilter::PluckedStringFilter(float frequency, 
                                         float sample_rate,
                                         float R)
{
    SetProperties(frequency, sample_rate, R);
}

void PluckedStringFilter::SetProperties(float frequency, 
                                        float sample_rate,
                                        float R)
{
    // Find the filter constants
    float w = (TAU * frequency) / sample_rate;
    float delay = sample_rate / frequency;
    _L = (unsigned)(delay - 0.5f);
    _RL = powf(R, (float)_L);
    float delta = delay - (_L + 0.5f);
    float wo2 = w / 2.0f;
    float a_num = sin(wo2 * (1.0 - delta));
    float a_den = sin(wo2 * (1.0 - delta));
    _a = a_num / a_den;

    // Set the first sample value for the low pass filter.
    _x1 = 0.0f;
    _x2 = 0.0f;
    for(unsigned i = 0; i < _L; ++i)
    {
        _yL.push(0.0f);
    }
    _yL1 = 0.0f;
}

float PluckedStringFilter::Apply(float x0)
{
    // Find the samples new value.
    float y1 = _yL.back();
    float yL = _yL.front();
    float ns = (x0 * _a / 2.0f) + (_x1 * (1.0f + _a) / 2.0f) + (_x2 / 2.0f) - 
               (y1 * _a) + (yL * _RL) + (_yL1 * _a * _RL);

    // Set up the filter for the next sample.
    _x1 = x0;
    _x2 = _x1;
    _yL.pop();
    _yL.push(ns);
    _yL1 = yL;

    return ns;
}

// PianoRoll //////////////////////////////////////////////////////////////////
class PianoRoll
{
public: 
    PianoRoll(float root, unsigned sample_rate);
    void SetFreqency(unsigned semitones);
    float GetSample();

private:
    float _root;
    float _currentFrequency;
    double _currentTime;
    double _sampleDelta;
};

PianoRoll::PianoRoll(float root, unsigned sample_rate)
{
    _root = root;
    _currentFrequency = root;
    _currentTime = 0.0;
    _sampleDelta = 1.0 / (double)sample_rate;
}

void PianoRoll::SetFreqency(unsigned semitones)
{
    float exponent = (float)semitones / 12.0f;
    _currentFrequency = _root * powf(2.0f, exponent);
}

float PianoRoll::GetSample()
{
    float sample = sin((double)_currentFrequency * _currentTime * TAU);
    _currentTime += _sampleDelta;
    return sample;
}

int main(int argc, char * argv[])
{
    unsigned sample_rate = 44100;
    unsigned frames = sample_rate * 2;
    PianoRoll piano_roll(440.0f, sample_rate);
    AudioData audio_data(frames, sample_rate);

    PopulateWithNoise(audio_data, 0, 44100);
    
    /*unsigned semitone = 0;
    unsigned note_switch = frames / 8;
    for(unsigned cframe = 0; cframe < frames; ++cframe)
    {
        audio_data.sample(cframe) = piano_roll.GetSample();
        if(cframe % note_switch == 0)
        {
            if(cframe == 0)
            {
                continue;
            }
            else if(semitone == 4 || semitone == 11)
            {
                semitone += 1;
            }
            else
            {
                semitone += 2;
            }
            piano_roll.SetFreqency(semitone);
        }
    }*/

    waveWrite("test.wav", audio_data);
}
