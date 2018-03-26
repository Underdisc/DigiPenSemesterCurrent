// cs245_h7.cpp
// -- articulate an audio sample using specified parameters
// cs245 3/17
//
// usage:
//   cs245_h7 <file>
// where:
//   <file> -- a text file (.txt extension) that contains the speed
//             up factor value and the loop points of an audio sample.
// notes:
//   The input WAVE file is assumed to have the same base name as
//   the text file.  For example, if the file 'fred.txt' is used
//   as the input text file, then the program will look for a file
//   named 'fred.wav' to use as the audio sample.  The audio sample
//   file is assumed to be 16 bit mono.
// output:
//   'cs245_h7.wav'

#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
using namespace std;


int main(int argc, char *argv[]) {
  if (argc != 2)
    return 0;

  ifstream textin(argv[1]);
  float factor;
  int loop_bgn, loop_end;
  textin >> factor >> loop_bgn >> loop_end;
  if (!textin) {
    cout << "bad text file" << endl;
    return 0;
  }

  stringstream ss(argv[1]);
  int i = ss.str().find('.');
  string fname = ss.str().substr(0,i) + ".wav";
  fstream in(fname.c_str(),ios_base::in|ios_base::binary);
  if (!in) {
    cout << "bad WAVE file: " << fname << endl;
    return 0;
  }

  char header[44];
  in.read(header,44);
  unsigned rate = *reinterpret_cast<unsigned*>(header+24),
           size = *reinterpret_cast<unsigned*>(header+40),
           count = size/2;
  short *samples_in = new short[count];
  in.read(reinterpret_cast<char*>(samples_in),size);

  int LOOP_COUNT = loop_end - loop_bgn,
      COUNT2 = max(5*LOOP_COUNT,int(rate)),
      COUNT1 = loop_bgn + COUNT2,
      COUNT = COUNT1 + COUNT2;
  float envelope = 1.0f,
        decay_const = (96.0f/20.0f)*log(10.0f)/COUNT2,
        decay_factor = exp(-decay_const);

  short *samples_out = new short[COUNT];

  for (int i=0; i < COUNT; ++i) {
    float findex = factor * i;
    findex = (findex > loop_end)
             ? loop_bgn + fmod(findex-loop_bgn,loop_end-loop_bgn)
             : findex;
    int k = int(findex);
    float t = findex - k,
          y = (1-t)*samples_in[k] + t*samples_in[k+1];
    samples_out[i] = short(envelope * y);
    envelope *= (i >= COUNT1) ? decay_factor : 1;
  }

  *reinterpret_cast<unsigned*>(header+4) = 36u + unsigned(2*COUNT);
  *reinterpret_cast<unsigned*>(header+40) = unsigned(2*COUNT);
  fstream out("cs245_h7.wav",ios_base::out|ios_base::binary);
  out.write(header,44);
  out.write(reinterpret_cast<char*>(samples_out),2*COUNT);

  delete[] samples_out;
  delete[] samples_in;
  return 0;
}

