#include "AudioData.h"

int main(void)
{
  AudioData ad("WaveTest.1.wav");
  waveWrite("test.wav", ad, 8);
  return 0;
}
