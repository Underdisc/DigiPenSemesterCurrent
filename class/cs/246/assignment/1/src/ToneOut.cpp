////////////////////////////////////////////////////////////////////////////////
// File:        ToneOut.cpp
// Class:       CS246
// Asssignment: 1
// Author:      Connor Deakin
// Email:       connor.deakin@digipen.edu
// Date:        2018-09-18
////////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <iostream>
#include <portaudio.h>
#include <sstream>
#include <string>

#include "Control.h"

// Tone ////////////////////////////////////////////////////////////////////////

// Structure used for controlling all parts of the tone that is emitted. This
// contains all of the controls for the frequency and amplitude of the tone.
class Tone
{
public:
    Tone() {}
    void Initialize(double sample_rate, double start_index, double frequency);
    void UpdateFrequency();
    void SetOctave(int octave);
    void SetPitch(double cents);
    void SetAmplitude(float decibels);
    double _sample_rate;
    double _index;
    double _delta_index;
    double _root_frequency;
    double _root_octave_frequency;
    double _pitch_multiplier;
    double _frequency;
    float _amplitude;
};

// Initializes the tone to starting values.
void Tone::Initialize(double sample_rate, double start_index, double frequency)
{
    _sample_rate = sample_rate;
    _index = start_index;
    _delta_index = frequency / sample_rate;
    _root_frequency = frequency;
    _root_octave_frequency = frequency;
    _pitch_multiplier = 1.0f;
    _frequency = frequency;
    _amplitude = 1.0f;
}

void Tone::UpdateFrequency()
{
    _frequency = _root_octave_frequency * _pitch_multiplier;
    _delta_index = _frequency / _sample_rate;
}

void Tone::SetOctave(int octave)
{
    int alpha = 1;
    for(int i = 1; i < octave; ++i)
    {
        alpha *= 2;
    }
    _root_octave_frequency = _root_frequency * (double)alpha;
    UpdateFrequency();
}


void Tone::SetPitch(double cents)
{
    double exp = cents / 1200.0f;
    _pitch_multiplier = std::pow(2.0f, exp);
    UpdateFrequency();
}

void Tone::SetAmplitude(float decibels)
{
    float exp = decibels / 20.0f;
    _amplitude = std::powf(10.0f, exp);
}

// ToneControl /////////////////////////////////////////////////////////////////

// Structure used for controlling the parameters of a Tone.
class ToneControl : public Control 
{
public:
    ToneControl();
    void Initialize(Tone* tone);
    void valueChanged(unsigned id, int value);
    Tone* _tone;
    int _values[3];
    char* _labels[3];
    char* _units[3];
};

ToneControl::ToneControl() : Control(3)
{
}

void ToneControl::Initialize(Tone* tone)
{
    // Set the starting tone.
    _tone = tone;

    // Set starting values, labels and units.
    _values[0] = 3;
    _values[1] = 0;
    _values[2] = 0;
    _labels[0] = "Octave:";
    _labels[1] = "Pitch:";
    _labels[2] = "Volume:";
    _units[0] = "";
    _units[1] = "cents";
    _units[2] = "dB";

    // Set ranges for all sliders.
    setRange(0, 1, 5);
    setRange(1, -1200, 1200);
    setRange(2, -1200, 1200);

    // Set starting values for all slides
    for(int i = 0; i < 3; ++i)
    {
        setValue(i, _values[i]);
    }

    // Set starting labels the octave and pitch sliders.
    std::stringstream ss;
    for(int i = 0; i < 2; ++i)
    {
        ss.str("");
        ss << _labels[i] << " " << _values[i] << " " << _units[i];
        std::string string_label;
        string_label = ss.str();
        setLabel(i, string_label.c_str());
    }

    // The value of the amplitude slider is set seperately because it is
    // represented as a floating point value to the user.
    ss.str("");
    ss << _labels[2] << " " << 0.0f << " " << _units[2];
    std::string string_label;
    string_label = ss.str();
    setLabel(2, string_label.c_str());
    
}

// Updates properties of the controlled tone when a value is changed.
void ToneControl::valueChanged(unsigned num, int value)
{
    // Update the property of the tone that has changed.
    if(num == 0)
    {
        _tone->SetOctave(value);
    }
    else if(num == 1)
    {
        double cents = (double)value;
        _tone->SetPitch(cents);
    }
    else if(num == 2)
    {
        // Convert the integer value to a floating point value.
        float decibels = (float)value;
        decibels = (decibels / 1200.0f) * 24.0f;
        _tone->SetAmplitude(decibels);

        // Update the value for the volume slider.
        std::stringstream ss;
        ss << _labels[num] << " " << decibels << " " << _units[num];
        std::string new_label = ss.str();
        setLabel(num, new_label.c_str());
    }

    // Update the value of the slider.
    _values[num] = value;

    // Update the label for the slider. We ignore an id of 2 since that label
    // will be updated above. This is necessary because this value must be
    // displayed as a floating point value.
    if(num != 2)
    {
        std::stringstream ss;
        ss << _labels[num] << " " << _values[num] << " " << _units[num];
        std::string new_label = ss.str();
        setLabel(num, new_label.c_str());
    }

    // Update the title with the new frequency.
    std::stringstream ss;
    ss << "ToneOut [" << _tone->_frequency << "]";
    std::string new_title = ss.str();
    setTitle(new_title.c_str());
}

// Main and Functions //////////////////////////////////////////////////////////

// Creates a triangle wave over a 0 to 1 interval for any value of x.
float Triangle(float x)
{
    x = x - floor(x);
    float sample;
    if(x  <= 0.5f)
    {
        sample = 1.0f - (4.0f * x);
    }
    else
    {
        x = x - 0.5f;
        sample = -1.0f + (4.0f * x);
    }
    return sample;

}

// The function called back by port audio for writing voltage values.
static int Write(const void* pa_in,
                 void* pa_out,
                 unsigned long frames,
                 const PaStreamCallbackTimeInfo* tinfo,
                 PaStreamCallbackFlags flags,
                 void* tone_data)
{
    // Convert parameters to correct types.
    float* out = reinterpret_cast<float*>(pa_out);
    Tone* tone = reinterpret_cast<Tone*>(tone_data);

    // Compute all samples requested by port audio.
    for(unsigned long i = 0; i < frames; ++i)
    {
        float value = Triangle((float)tone->_index);
        out[i] = tone->_amplitude * value; 
        tone->_index += tone->_delta_index;
    }

    return paContinue;
}

// Entry point.
int main(void)
{
    Tone tone;
    tone.Initialize(44100.0, 0.25, 55.0);

    // Initialize port audio.
    Pa_Initialize();

    // Create output parameters.
    PaStreamParameters params;
    params.device = Pa_GetDefaultOutputDevice();
    params.channelCount = 1;
    params.sampleFormat = paFloat32;
    params.suggestedLatency = Pa_GetDeviceInfo(params.device)
                                  ->defaultHighOutputLatency;
    params.hostApiSpecificStreamInfo = 0;

    // Open output stream.
    PaStream* stream;
    Pa_OpenStream(&stream,
                  0,
                  &params,
                  tone._sample_rate,
                  0,
                  paNoFlag,
                  Write,
                  &tone);
    Pa_StartStream(stream);

    // Launch the editor.
    ToneControl* tone_control = new ToneControl();
    tone_control->Initialize(&tone);
    tone_control->show(true);

    // Wait for user input before closing.
    std::cin.get();

    // Close the editor.
    tone_control->show(false);
    delete tone_control;

    // Close the stream and terminate port auido.
    Pa_StopStream(stream);
    Pa_CloseStream(stream);
    Pa_Terminate();

    return 0;
}
