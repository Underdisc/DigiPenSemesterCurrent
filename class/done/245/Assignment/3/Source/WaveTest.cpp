// WaveTest.cpp
// -- some simple tests of AudioData WAVE functions
// cs245 1/18

#include <iostream>
#include <fstream>
#include <cmath>
#include <cstring>
#include <exception>
#include "AudioData.h"
using namespace std;


const float PI = 4.0f * atan(1.0f);


int main(void) {

  { // write a 2 second sine wave (16 bit mono)
    const unsigned R = 44100;
    const float f = 220.0f;
    AudioData ad(2*R,R);
    for (unsigned i=0; i < ad.frames(); ++i)
      ad.data()[i] = sin(2*PI*f*i/R);
    if (waveWrite("WaveTest.1.wav",ad))
      cout << "file 'WaveTest.1.wav' written" << endl;
    else
      cout << "failed to write file 'WaveTest.1.wav'" << endl;
  }

  { // write 1.5 second sine waves (16 bit stereo)
    const unsigned R = 22050;
    const float fL = 220.0f,
                fR = 222.0f;
    unsigned frames = unsigned(1.5f*R);
    AudioData ad(frames,R,2);
    for (unsigned i=0; i < ad.frames(); ++i) {
      ad.sample(i,0) = sin(2*PI*fL*i/R);
      ad.sample(i,1) = sin(2*PI*fR*i/R);
    }
    if (waveWrite("WaveTest.2.wav",ad))
      cout << "file 'WaveTest.2.wav' written" << endl;
    else
      cout << "failed to write file 'WaveTest.2.wav'" << endl;
  }

  { // write a 0.7 second sine wave (8 bit mono)
    const unsigned R = 11025;
    const float f = 123.5f;
    unsigned frames = unsigned(0.7f*R);
    AudioData ad(frames,R);
    for (unsigned i=0; i < ad.frames(); ++i)
      ad.data()[i] = sin(2*PI*f*i/R);
    if (waveWrite("WaveTest.3.wav",ad,8))
      cout << "file 'WaveTest.3.wav' written" << endl;
    else
      cout << "failed to write file 'WaveTest.3.wav'" << endl;
  }

  { // write 1 second sine waves (8 bit stereo)
    const unsigned R = 48000;
    const float fL = 220.0f,
                fR = 293.7f;
    AudioData ad(R,R,2);
    for (unsigned i=0; i < ad.frames(); ++i) {
      ad.sample(i,0) = 0.9f*sin(2*PI*fL*i/R);
      ad.sample(i,1) = 0.7f*sin(2*PI*fR*i/R);
    }
    if (waveWrite("WaveTest.4.wav",ad,8))
      cout << "file 'WaveTest.4.wav' written" << endl;
    else
      cout << "failed to write file 'WaveTest.4.wav'" << endl;
  }


  // read/write a 16 bit mono file
  cout << "reading 'WaveTest.1.wav'" << endl;
  try {
    AudioData ad("WaveTest.1.wav");
    float duration = float(ad.frames())/float(ad.rate());
    cout << "duration: " << duration << "s" << endl;
    for (unsigned i=0; i < ad.frames(); ++i)
      ad.data()[i] = -0.8f*ad.data()[i];
    if (waveWrite("WaveTest.5.wav",ad))
      cout << "file 'WaveTest.5.wav' written" << endl;
    else
      cout << "failed to write file 'WaveTest.5.wav'" << endl;
  }
  catch (exception &e) {
    cout << "failed: " << e.what() << endl;
  }
 
  // read/write a 16 bit stereo file
  cout << "reading 'WaveTest.2.wav'" << endl;
  try {
    AudioData ad("WaveTest.2.wav");
    float duration = float(ad.frames())/float(ad.rate());
    cout << "duration: " << duration << "s" << endl;
    for (unsigned i=0; i < ad.frames(); ++i) {
      ad.sample(i,0) =  0.8f*ad.sample(i,0);
      ad.sample(i,1) = -0.5f*ad.sample(i,1);
    }
    if (waveWrite("WaveTest.6.wav",ad))
      cout << "file 'WaveTest.6.wav' written" << endl;
    else
      cout << "failed to write file 'WaveTest.6.wav'" << endl;
  }
  catch (exception &e) {
    cout << "failed: " << e.what() << endl;
  }

  // read/write a 8 bit mono file
  cout << "reading 'WaveTest.3.wav'" << endl;
  try {
    AudioData ad("WaveTest.3.wav");
    float duration = float(ad.frames())/float(ad.rate());
    cout << "duration: " << duration << "s" << endl;
    for (unsigned i=0; i < ad.frames(); ++i)
      ad.data()[i] = -0.9f*ad.data()[i];
    if (waveWrite("WaveTest.7.wav",ad,8))
      cout << "file 'WaveTest.7.wav' written" << endl;
    else
      cout << "failed to write file 'WaveTest.7.wav'" << endl;
  }
  catch (exception &e) {
    cout << "failed: " << e.what() << endl;
  }

  // read/write a 8 bit stereo file
  cout << "reading 'WaveTest.4.wav'" << endl;
  try {
    AudioData ad("WaveTest.4.wav");
    float duration = float(ad.frames())/float(ad.rate());
    cout << "duration: " << duration << "s" << endl;
    for (unsigned i=0; i < ad.frames(); ++i) {
      ad.sample(i,0) =  0.8f*ad.sample(i,0);
      ad.sample(i,1) = -ad.sample(i,1);
    }
    if (waveWrite("WaveTest.8.wav",ad,8))
      cout << "file 'WaveTest.8.wav' written" << endl;
    else
      cout << "failed to write file 'WaveTest.8.wav'" << endl;
  }
  catch (exception &e) {
    cout << "failed: " << e.what() << endl;
  }

  return 0;
}

