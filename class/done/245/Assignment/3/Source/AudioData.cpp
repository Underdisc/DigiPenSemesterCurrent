/*****************************************************************************/
/*!
\file AudioData.cpp
\author Connor Deakin
\par E-mail: connortdeakin\@gmail.com
\par Assignment: 2
\par Course: CS 245
\par Term: Spring 2018
\date 23/01/2018
\brief
  Contains the implementation of the audio data interface
*/
/*****************************************************************************/

#include <math.h>

#include "AudioData.h"

typedef unsigned int uint;

/*!
\brief AudioData Constructor
\param nframes The number of frames
\param R The sampling rate
\param nchannels The number of channels in the audio data
*/
AudioData::AudioData(unsigned nframes, unsigned R, unsigned nchannels) :
  fdata(), frame_count(nframes), sampling_rate(R), channel_count(nchannels)
{
  fdata.resize(frame_count * channel_count);
}

/*!
\brief Returns the sample value at the given frame and channel.
\param frame The frame
\param channel The channel
\return Read brief
*/
float AudioData::sample(unsigned frame, unsigned channel) const
{
  unsigned index = frame * channel_count + channel;
  return fdata[index];
}

/*!
\brief Returns a reference to the sample value at the given frame and channel.
\param frame The frame
\param channel The channel
\return Read brief
*/
float & AudioData::sample(unsigned frame, unsigned channel)
{
  unsigned index = frame * channel_count + channel;
  return fdata[index];
}

/*****************************************************************************/
/*!
\brief
  Normalizes the audio data and scales it by a specified decibel value.

\param ad The AudioData to be normalized.
\param dB The amount in decibles that the audio data
  will be seperated from unity.
*/
/*****************************************************************************/
void normalize(AudioData &ad, float dB)
{
  unsigned num_frames = ad.frames();
  unsigned num_channels = ad.channels();
  // calculate dc offsets for each channel
  std::vector<float> dc_offset;
  dc_offset.resize(num_channels, 0.0f);
  for(uint i = 0; i < num_frames; ++i)
  {
    for(uint j = 0; j < num_channels; ++j)
      dc_offset[j] += ad.sample(i, j);
  }
  for(uint i = 0; i < num_channels; ++i)
    dc_offset[i] /= num_frames;
  // subrtract dc offsets for all samples
  // and find the largest sample value
  float largest_sample = 0.0f;
  for(uint i = 0; i < num_frames; ++i)
  {
    for(uint j = 0; j < num_channels; ++j)
    {
      float & sample = ad.sample(i, j);
      sample-= dc_offset[j];
      float abs_sample = abs(sample);
      if(abs_sample > largest_sample)
        largest_sample = abs_sample;
    }
  }
  // calculate gain factor used for the new max sample value
  float linear_gain = pow(10.0f, dB / 20.0f);
  linear_gain =  linear_gain / largest_sample;
  // scale all samples by linear gain
  for(uint i = 0; i < num_frames; i++)
  {
    for(uint j = 0; j < num_channels; ++j)
      ad.sample(i, j) *= linear_gain;
  }
}
