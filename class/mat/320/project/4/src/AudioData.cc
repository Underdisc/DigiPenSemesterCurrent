
#include <cstring>
#include <climits>
#include <exception>
#include <fstream>
#include <math.h>
#include <string>

#include "AudioData.h"

typedef unsigned char uchar;
typedef short int int16;
typedef unsigned int uint32;
typedef unsigned int uint;
typedef unsigned short int uint16;

#define CHUNKHEADER_SIZE 8
#define LABEL_SIZE 4
#define SIZE_SIZE 4
#define FILETYPE_SIZE 4
#define FMT_SIZE 16

// A chunk header
struct Chunk
{
  Chunk() {}
  char label[LABEL_SIZE];
  uint32 size;
};

/*!
\brief Used to throw an error when there is a problem with the wave file.
\param what A message describing the error.
*/
void ThrowFileError(const char * what, const char * fname)
{
  std::string message(what);
  message.append(": ");
  message.append(fname);
  std::runtime_error error(message);
  throw(error);
}

/*!
\brief Given that in_file is located at the start of a chunk, this will move
  to the chunk with the given label.
\param in_file The file stream that is to be moved to a new chunk
\param label_name The label of the chunk that is to be moved to
\param found_chunk This will be filled with the found chunks header information
\return true if the chunk is found, otherwise false.
*/
bool MoveToChunk(std::ifstream * in_file, const char * label_name,
  Chunk * found_chunk)
{
  while(!in_file->eof())
  {
    in_file->read(reinterpret_cast<char *>(found_chunk), CHUNKHEADER_SIZE);
    if(strncmp(found_chunk->label, label_name, LABEL_SIZE) == 0)
      return true;
    in_file->seekg(found_chunk->size, std::ios_base::cur);
  }
  return false;
}

/*****************************************************************************/
/*!
\brief
  Reads in a WAVE file.
\param fname
  The name of the WAVE file to be read.
*/
/*****************************************************************************/
AudioData::AudioData(const char * fname)
{
  // open wave file
  std::ifstream in_file;
  in_file.open(fname, std::ifstream::in);
  // check for failiure to open
  if(!in_file.is_open())
    ThrowFileError("Failed to open file", fname);

  // read in first header (RIFF)
  Chunk master_chunk;
  in_file.read(reinterpret_cast<char *>(&master_chunk), CHUNKHEADER_SIZE);
  // make sure it's a RIFF file
  if(strncmp(master_chunk.label, "RIFF", LABEL_SIZE) != 0)
    ThrowFileError("File is not in RIFF format", fname);

  // make sure it's a WAVE file
  char temp_buffer[FILETYPE_SIZE];
  in_file.read(temp_buffer, FILETYPE_SIZE);
  if(strncmp(temp_buffer, "WAVE", FILETYPE_SIZE) != 0)
    ThrowFileError("File is not a WAVE file", fname);

  // find format chunk
  Chunk cur_chunk;
  bool chunk_found = MoveToChunk(&in_file, "fmt ", &cur_chunk);
  if(!chunk_found)
    ThrowFileError("File is corrupted", fname);
  // read in format data
  char fmt[FMT_SIZE];
  in_file.read(fmt, FMT_SIZE);
  channel_count = *reinterpret_cast<uint16 *>(fmt + 2);
  sampling_rate = *reinterpret_cast<uint32 *>(fmt + 4);
  uint16 bit_resolution = *reinterpret_cast<uint16 *>(fmt + 14);

  // find data chunk
  chunk_found = MoveToChunk(&in_file, "data", &cur_chunk);
  if(!chunk_found)
    ThrowFileError("File is corrupted", fname);
  // read in data
  uint32 frame_size = channel_count * (bit_resolution / 8);
  frame_count = cur_chunk.size / frame_size;
  uint32 total_samples = channel_count * frame_count;
  fdata.resize(total_samples);

  for(unsigned int s = 0; s < total_samples; ++s)
  {
    if(bit_resolution == 16)
    {
      int16 sample;
      in_file.read(reinterpret_cast<char *>(&sample), sizeof(int16));
      fdata[s] = static_cast<float>(sample) / static_cast<float>(SHRT_MAX);
    }
    else if(bit_resolution == 8)
    {
      uchar sample;
      in_file.read(reinterpret_cast<char *>(&sample), sizeof(uchar));
      fdata[s] = static_cast<float>(sample - 127) / 128.0f;
    }
  }
}


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

// The header used for writing a WAVE file
struct WaveHeader
{
  char riff_header[4];
  uint32 riff_size;
  char wave_header[4];
  char format_header[4];
  uint32 format_size;
  uint16 audio_format;
  uint16 channels;
  uint32 sampling_rate;
  uint32 bytes_per_second;
  uint16 bytes_per_frame;
  uint16 bits_per_sample;
  char data_header[4];
  uint32 data_size;

};

/*****************************************************************************/
/*!
\brief
  Writes the data contained in ad to a WAVE file.
\param fname
  The name of the wave file that will be written to
\param ad
  The audio data that will be written to the wave file
\param bits
  The bit rate the audio data will be stored with
\return True is the write is successful, false otherwise
*/
/*****************************************************************************/
bool waveWrite(const char * fname, const AudioData &ad, unsigned bits)
{
  uint16 bytes_per_frame = ad.channels() * (bits / 8);
  uint32 bytes_per_second = ad.rate() * bytes_per_frame;
  uint32 data_size = ad.frames() * bytes_per_frame;
  // filling out the header
  WaveHeader header = {
    {'R','I','F','F'},
    data_size + 36,
    {'W','A','V','E'},
    {'f','m','t',' '},
    FMT_SIZE,
    1,
    static_cast<uint16>(ad.channels()),
    ad.rate(),
    bytes_per_second,
    bytes_per_frame,
    static_cast<uint16>(bits),
    {'d','a','t','a'},
    data_size,
  };
  // opening file and writing header
  std::fstream out_file;
  out_file.open(fname, std::ofstream::out | std::fstream::binary);
  out_file.write(reinterpret_cast<char *>(&header), sizeof(header));
  // writing data to file
  char * data = new char[data_size];
  const float * sample_data = ad.data();
  uint num_samples = ad.frames() * ad.channels();
  // 8 bit resolution
  if(bits == 8)
  {
    uchar * uchar_data = reinterpret_cast<uchar *>(data);
    for(uint s = 0; s < num_samples; ++s)
    {
      uchar_data[s] = static_cast<uchar>(sample_data[s] * 127.0f + 128.0f);
    }

  }
  // 16 bit resolution
  else if(bits == 16)
  {
    int16 * int16_data = reinterpret_cast<int16 *>(data);
    float max = static_cast<float>(SHRT_MAX);
    for(uint s = 0; s < num_samples; ++s)
    {
      int16_data[s] = static_cast<int16>(sample_data[s] * max);
    }
  }
  else
  {
    return false;
  }
  out_file.write(data, data_size);
  return true;
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
      sample -= dc_offset[j];
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
