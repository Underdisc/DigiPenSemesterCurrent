/*****************************************************************************/
/*!
\file Resample.cpp
\author Connor Deakin
\par E-mail: connortdeakin\@gmail.com
\par Assignment: 4
\par Course: CS 245
\par Term: Spring 2018
\date 12/02/2018
\brief
  Contains the implementation of the Resample class. Used for resampling audio
  data at modified playback rates.
*/
/*****************************************************************************/

#include <cmath>
#include "Resample.h"

#define OCTAVE_CENTS 1200.0f

Resample::Resample(const AudioData * ad_ptr, unsigned channel, float factor,
  unsigned loop_bgn, unsigned loop_end) :
  audio_data(ad_ptr), ichannel(channel), findex(0.0f), findex_increment(factor),
  findex_multiplier(1.0f), iloop_bgn(loop_bgn), iloop_end(loop_end)
{}

float Resample::operator()(void)
{
  // get integer and fractional parts of findex
  unsigned int_part = static_cast<unsigned>(findex);
  double frac_part = findex - static_cast<double>(int_part);

  // account for !looping / looping
  bool looping = (iloop_bgn < iloop_end);
  if(!looping)
  {
    if (int_part >= audio_data->frames())
    {
      return 0.0f;
    }
  }
  else if(int_part >= iloop_end)
  {
    unsigned looped_samples = iloop_end - iloop_bgn;
    int_part = (int_part - iloop_bgn) % looped_samples;
    int_part += iloop_bgn;
  }
  // find frame numbers for start and end samples
  unsigned start_i = int_part;
  unsigned end_i = int_part + 1;
  if(looping && end_i >= iloop_end)
    end_i = iloop_bgn;
  // find resampled value
  float start_sample = audio_data->sample(start_i, ichannel);
  float end_sample = audio_data->sample(end_i, ichannel);
  float resample = start_sample + frac_part * (end_sample - start_sample);
  // update findex
  findex = findex + findex_increment * findex_multiplier;
  return resample;
}

void Resample::offsetPitch(float cents)
{
  if(cents == 0.0f)
  {
    findex_multiplier = 1.0f;
    return;
  }
  float exponent = cents / OCTAVE_CENTS;
  findex_multiplier = std::pow(2.0f, exponent);
}
