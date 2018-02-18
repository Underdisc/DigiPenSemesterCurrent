#include "AudioData.h"
#include <cmath>

#define PI 3.14159265359f

int main(void)
{

  unsigned R = 44100;
  unsigned frames = R * 2;
  float f = 440.0f;
  AudioData ad(frames, R, 1);

  for(unsigned frame = 0; frame < frames; ++frame)
  {
    float time = (float)frame / (float)R;
    float done = (float)frame / (float)frames;
    done = done * done;
    done = 1.0f - done;

    float sample_value = sin(time * PI * 2.0f * f);

    if(sample_value < 0.0f)
    {
      sample_value = -1.0f * done;
    }
    else
    {
      sample_value = 1.0f * done;
    }
    ad.sample(frame) = sample_value;
  }
  waveWrite("A_Square_Inverse_Quadratic.wav", ad);
  return 0;
}
