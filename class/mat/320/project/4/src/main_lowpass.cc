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
#include <string>

#include "AudioData.h"

class LowPassFilter
{
public:
    LowPassFilter(float a);
    void Apply(AudioData & audio_data);
private:
    float _a;
    float _xm1;
};

LowPassFilter::LowPassFilter(float a) : _a(a), _xm1(0.0f)
{}

void LowPassFilter::Apply(AudioData & audio_data)
{
    unsigned frames = audio_data.frames();
    for(unsigned i = 0; i < frames; ++i)
    {
        float sample = audio_data.sample(i);
        float new_sample = sample + _a * _xm1;
        _xm1 = sample;
        audio_data.sample(i) = new_sample;
    }
}

int main(int argc, char * argv[])
{
    if(argc != 4)
    {
        std::cout << "Incorrect number of command line arguments." << std::endl
                  << "./lowpass.exe a n filename" << std::endl;
        return 0;
    }

    float a = (float)atof(argv[1]); 
    unsigned n = (unsigned)atoi(argv[2]);
    std::string input_filename(argv[3]);
    std::string output_filename("output.wav");

    try
    {
        AudioData audio_data(input_filename.c_str());
        LowPassFilter lp(a);
        for(unsigned i = 0; i < n; ++i)
        {
            lp.Apply(audio_data);
            normalize(audio_data, -1.5f);
        }
        waveWrite(output_filename.c_str(), audio_data);
    }
    catch(const std::runtime_error & error)
    {
        std::cout << error.what() << std::endl;
        return 0;
    }
}
