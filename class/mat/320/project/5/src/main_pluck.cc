/*****************************************************************************/
/*!
\file main_pluck.cc
\author Connor Deakin
\par E-mail: connortdeakin\@digipen.edu
\par Project: 5
\par Course: MAT 320
\date 2018/12/5
\brief
    Contains an implementation of the plucked string filter that will produce
    a wave file that plays the major scale.

Usage:
./pluck.exe f
f - The root frequency that the major scale will be played from.
*/
/*****************************************************************************/

#include <iostream>
#include <math.h>
#include <queue>

#include "AudioData.h"

#define TAU 6.28318530717958647692

float Random()
{
    float random = (float)rand() / (float)RAND_MAX;
    random = (random * 2.0f) - 1.0f;
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
    // Comb filter values. x -> g
    unsigned _L_delay;
    float _RL;
    std::queue<float> _yL;

    // Low pass filter values. g -> h
    float _g1;

    // All pass filter values. h -> y
    float _a;
    float _h1;
    float _y1;
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
    float delay = sample_rate / frequency;
    _L_delay = (unsigned)(delay - 0.5f);
    _RL = powf(R, (float)_L_delay);
    float delta = delay - (_L_delay + 0.5f);
    float w0 = (TAU * frequency) / sample_rate;
    float a_num = sin((w0 / 2.0f) * (1.0 - delta));
    float a_den = sin((w0 / 2.0f) * (1.0 + delta));
    _a = a_num / a_den;

    // Set the stored sample values to zero.
    while(!_yL.empty())
    {
        _yL.pop();
    }
    for(unsigned i = 0; i < _L_delay; ++i)
    {
        _yL.push(0.0f);
    }
    _g1 = 0.0f;
    _h1 = 0.0f;
}

float PluckedStringFilter::Apply(float x0)
{
    // Apply the comb filter.
    float yL = _yL.front();
    float g0 = x0 + _RL * yL;

    // Apply the low pass filter.
    float h0 = g0 / 2.0f + _g1 / 2.0f;
    
    // Apply the all pass filter.
    float y1 = _yL.back();
    float y0 = _a * h0 + _h1 - _a * y1;


    // Set up the filter for the next sample.
    _yL.pop();
    _yL.push(y0);
    _g1 = g0;
    _h1 = h0;

    return y0;
}

int main(int argc, char * argv[])
{ 
    if(argc != 2)
    {
        std::cout << "Incorrect number of arguments." << std::endl
                  << "./pluck f" << std::endl;
        return 0;
    }

    // Create filter
    float root = (float)atof(argv[1]);
    unsigned sample_rate = 44100;
    float R = 0.99985f;
    PluckedStringFilter filter(root, sample_rate, R);

    // Create the audio data that will be written to.
    unsigned frames = sample_rate * 8;
    AudioData audio_data(frames, sample_rate);

    // Create ascending major scale
    unsigned semitone = 0;
    unsigned note_switch = frames / 8;
    for(unsigned cframe = 0; cframe < frames; ++cframe)
    {
        audio_data.sample(cframe) = filter.Apply(audio_data.sample(cframe));
        if(cframe % note_switch == 0)
        {
            // Populate the buffer with noise when we switch notes and go to
            // the next note in the scale.
            PopulateWithNoise(audio_data, cframe, 100);
            if(cframe == 0)
            {
                continue;
            }
            // Half steps occur after the 3rd and 7th.
            else if(semitone == 4 || semitone == 11)
            {
                semitone += 1;
            }
            // Otherwise, whole steps are performed.
            else
            {
                semitone += 2;
            }
            // Update the output frequency of the filter.
            float exponent = (float)semitone / 12.0f;
            float frequency = root * powf(2.0f, exponent);
            filter.SetProperties(frequency, sample_rate, R);
        }
    }

    // Write the audio data to file.
    waveWrite("pluck.wav", audio_data);
}
