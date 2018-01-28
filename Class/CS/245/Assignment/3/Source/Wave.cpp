#include <string>
#include <cstring>
#include <fstream>
#include <exception>
#include <iostream>

#include "AudioData.h"

typedef unsigned int uint32;
typedef unsigned short int uint16;

#define CHUNKHEADER_SIZE 8
#define LABEL_SIZE 4
#define SIZE_SIZE 4
#define FILETYPE_SIZE 4

struct Chunk
{
  Chunk() : offset(0) {}
  char label[LABEL_SIZE];
  uint32 size;
  uint32 offset;
};

void ThrowFileError(const char * what, const char * fname)
{
  std::string message(what);
  message.append(": ");
  message.append(fname);
  std::runtime_error error(message);
  throw(error);
}

bool MoveToChunk(std::ifstream * in_file, const char * label_name,
  Chunk * master_chunk, Chunk * found_chunk)
{
  std::cout << "START OFFSET: " << master_chunk->offset << " -----> ";
  while(!in_file->eof())
  {
    if(master_chunk->offset >=  master_chunk->size)
    {
      std::cout << "this happened" << std::endl;
      return false;
    }
    in_file->read(reinterpret_cast<char *>(master_chunk), CHUNKHEADER_SIZE);

    std::cout << "|" << found_chunk->label << "| +" << found_chunk->size + CHUNKHEADER_SIZE << std::endl;

    master_chunk->offset += CHUNKHEADER_SIZE + found_chunk->size;
    if(strncmp(found_chunk->label, label_name, LABEL_SIZE) == 0)
      return true;
    in_file->seekg(found_chunk->size, std::ios_base::cur);
    std::cout << "OFFSET: " << master_chunk->offset << std::endl;
  }
  std::cout << "WRONG thing happened" << std::endl;
  return false;
}

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
  std::cout << "MASTER CHUNK SIZE: " << master_chunk.size << std::endl;

  // make sure it's a WAVE file
  char temp_buffer[FILETYPE_SIZE];
  in_file.read(temp_buffer, FILETYPE_SIZE);
  if(strncmp(temp_buffer, "WAVE", FILETYPE_SIZE) != 0)
    ThrowFileError("File is not a WAVE file", fname);
  master_chunk.offset += FILETYPE_SIZE;

  // find format chunk
  Chunk cur_chunk;
  bool chunk_found = MoveToChunk(&in_file, "fmt ", &master_chunk, &cur_chunk);
  if(!chunk_found)
    ThrowFileError("File is corrupted", fname);

  in_file.seekg(sizeof(uint16), std::ios_base::cur);
  in_file.read(temp_buffer, sizeof(uint16));
  channel_count = *reinterpret_cast<uint16 *>(temp_buffer);
  in_file.read(temp_buffer, sizeof(uint32));
  sampling_rate = *reinterpret_cast<uint32 *>(temp_buffer);

  chunk_found = MoveToChunk(&in_file, "data", &master_chunk, &cur_chunk);
  if(!chunk_found)
    ThrowFileError("File is corrupted", fname);

  std::cout << "Channels: " << channel_count << std::endl;
  std::cout << "R: " << sampling_rate << std::endl;
  // fill out format fields


  std::cout << "We made it";
}

bool waveWrite(const char * fname, const AudioData &ad, unsigned bits)
{
  return false;
}
