// AudioDataTest.cpp
// -- some simple test of AudioData class & Normalize function
// cs245 1/18

#include <iostream>
#include <fstream>
#include <cmath>
#include <cstring>
#include "AudioData.h"
using namespace std;


const float PI = 4.0f * atan(1.0f);
const float MAX16 = float((1<<15)-1);


int main(void) {

  // WAVE file header (some fields not filled in)
  char header[44];
  strncpy(header+0,"RIFF",4);
  strncpy(header+8,"WAVE",4);
  strncpy(header+12,"fmt ",4);
  *reinterpret_cast<unsigned*>(header+16) = 16;
  *reinterpret_cast<unsigned short*>(header+20) = 1;
  *reinterpret_cast<unsigned short*>(header+34) = 16;
  strncpy(header+36,"data",4);

  { // 2 second mono sample
    const unsigned R = 44100;
    const float f[3] = { 220.0f, 221.0f, 329.6f };
    const unsigned count = 2*R;
    AudioData ad(count);
    for (unsigned i=0; i < count; ++i) {
      float t = float(i)/float(R);
      ad.data()[i] = 0.8f*sin(2*PI*f[0]*t)
                     + 0.7f*sin(2*PI*f[1]*t)
                     + 0.4f*sin(2*PI*f[2]*t)
                     + 0.21f;
    }
    normalize(ad,-1.2f);
    unsigned size = 2*count;
    *reinterpret_cast<unsigned*>(header+4) = 36u + size;
    *reinterpret_cast<unsigned short*>(header+22) = 1;
    *reinterpret_cast<unsigned*>(header+24) = R;
    *reinterpret_cast<unsigned*>(header+28) = 2*R;
    *reinterpret_cast<unsigned short*>(header+32) = 2;
    *reinterpret_cast<unsigned*>(header+40) = size;
    fstream out("AudioDataTest.1.wav",ios_base::binary|ios_base::out);
    short *samples = new short[count];
    for (unsigned i=0; i < count; ++i)
      samples[i] = short(MAX16*ad.data()[i]);
    out.write(header,44);
    out.write(reinterpret_cast<char*>(samples),size);
    delete[] samples;
  }

  { // 1.5 second stereo sample
    const unsigned R = 22050;
    const float f[4] = { 220.0f, 110.0f, 100.0f, 329.6f };
    const unsigned count = unsigned(1.5f*R);
    AudioData ad(count,R,2);
    for (unsigned i=0; i < count; ++i) {
      float t = float(i)/float(R),
            phiL = 10*(t/1.5f)*sin(2*PI*f[0]*t),
            phiR = 4*0.5f*(1-cos(2*PI*t/1.5f))*sin(2*PI*f[3]*t);
      ad.sample(i,0) = -0.1f + 0.3f*sin(2*PI*f[0]*t + phiL);
      ad.sample(i,1) = 0.5f + 0.1f*sin(2*PI*f[1]*t + phiR)
                            + 0.1f*sin(2*PI*f[2]*t + phiR);
    }
    normalize(ad,-3.7f);
    unsigned size = 2*2*count;
    *reinterpret_cast<unsigned*>(header+4) = 36u + size;
    *reinterpret_cast<unsigned short*>(header+22) = 2;
    *reinterpret_cast<unsigned*>(header+24) = R;
    *reinterpret_cast<unsigned*>(header+28) = 4*R;
    *reinterpret_cast<unsigned short*>(header+32) = 4;
    *reinterpret_cast<unsigned*>(header+40) = size;
    fstream out("AudioDataTest.2.wav",ios_base::binary|ios_base::out);
    unsigned length = 2*count;
    short *samples = new short[length];
    for (unsigned i=0; i < length; ++i)
      samples[i] = short(MAX16*ad.data()[i]);
    out.write(header,44);
    out.write(reinterpret_cast<char*>(samples),size);
    delete[] samples;
  }

  return 0;
}

