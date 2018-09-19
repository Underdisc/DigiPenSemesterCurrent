#include <cmath>
#include <iostream>
#include <portaudio.h>
#include <sstream>
#include <string>

#include "Control.h"

class WaveControl : public Control 
{
public:
    WaveControl(void);
    void valueChanged(unsigned id, int value);
    int values[3];
    char* labels[3];
    char* units[3];
};

WaveControl::WaveControl(void) : Control(3)
{
    values[0] = 3;
    values[1] = 0;
    values[2] = 0;
    labels[0] = "Octave:";
    labels[1] = "Pitch:";
    labels[2] = "Volume:";
    units[0] = "";
    units[1] = "cents";
    units[2] = "dB";

    // Set starting lables and values for all sliders.
    std::stringstream ss;
    std::string string_labels[3];
    for(int i = 0; i < 3; ++i)
    {
        ss.str("");
        ss << labels[i] << " " << values[i] << " " << units[i];
        string_labels[i] = ss.str();
        setValue(i, values[i]);
        setLabel(i, string_labels[i].c_str());
    }

    // Set ranges for all sliders.
    setRange(0, 0, 5);
    setRange(1, -1200, 1200);
    setRange(2, -24, 24);
}

void WaveControl::valueChanged(unsigned num, int value)
{
}

// Tracks where we are in the current wave.
struct Wave
{
    double index;
    double delta_index;
};

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
                 void* wave_data)
{
    // Convert parameters to correct types.
    float* out = reinterpret_cast<float*>(pa_out);
    Wave* wave = reinterpret_cast<Wave*>(wave_data);

    // Compute all samples requested by port audio.
    for(unsigned long i = 0; i < frames; ++i)
    {
        float value = Triangle(wave->index);
        out[i] = value; 
        wave->index += wave->delta_index;
    }

    return paContinue;
}


// Entry point.
int main(void)
{
    // Define sample rate and current wave. We will begin a quarter of the way
    // through the wave for the first wave to prevent a starting click.
    const double sample_rate = 44100;
    Wave wave = {0.25, 220.00 / sample_rate};

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
    Pa_OpenStream(&stream, 0, &params, sample_rate, 0, paClipOff, Write, &wave);
    Pa_StartStream(stream);

    // Launch the editor.
    WaveControl* wave_control = new WaveControl();
    wave_control->show(true);

    // Wait for user input before closing.
    std::cin.get();

    // Close the editor.
    wave_control->show(false);
    delete wave_control;

    // Close the stream and terminate port auido.
    Pa_StopStream(stream);
    Pa_CloseStream(stream);
    Pa_Terminate();

    return 0;
}
