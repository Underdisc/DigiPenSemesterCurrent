#include <cstring>
#include "../Utility/Error.h"

#include "Mesh.h"

#define READ_BUFFERSIZE 50
#define DATA_OFFSET 2
#define NULL_TERMINATOR '\0'
#define VERTEX_CHARACTER 'v'
#define INDEX_CHARACTER 'f'
#define VALUE_DELIMETER ' '

Mesh::Mesh(const std::string & file_name, FileType type)
{
  switch (type){
  case OBJ:
    LoadObj(file_name);
    break;
  default:
    Error error("Mesh.cpp", "Mesh Constructor");
    error.Add("The Mesh class cannot load this file type");
    ErrorLog::Write(error);
  }
}

void * Mesh::VertexData()
{
  return (void *)_vertices.data();
}

void * Mesh::IndexData()
{
  return (void *)_indicies.data();
}

unsigned Mesh::IndexDataSize()
{
  return _indicies.size();
}

unsigned Mesh::VertexDataSizeBytes()
{
  return _vertices.size() * sizeof(float);
}

unsigned Mesh::IndexDataSizeBytes()
{
  return _indicies.size() * sizeof(unsigned);
}


inline void split_string(std::vector<char *> * string_start, char * string)
{
  unsigned i = 0;
  string_start->push_back(string);
  while (string[i] != NULL_TERMINATOR) {
    if (string[i] == VALUE_DELIMETER) {
      string[i] = NULL_TERMINATOR;
      ++i;
      string_start->push_back(string + i);
    }
    ++i;
  }
}

inline void Mesh::LoadObj(const std::string & file_name)
{
  std::ifstream file_stream(file_name.c_str());
  if (!file_stream.is_open()) {
    Error error("Mesh.cpp", "LoadObj");
    error.Add("File failed to open.");
    error.Add(file_name.c_str());
    ErrorLog::Write(error);
    return;
  }
  // TODO - Reserve space in vectors
  while (!file_stream.eof()){
    char line[READ_BUFFERSIZE];
    file_stream.getline(line, READ_BUFFERSIZE);
    std::vector<char *> string_values;
    unsigned num_values;
    switch (line[0]) {
    case VERTEX_CHARACTER:
      split_string(&string_values, line + DATA_OFFSET);
      num_values = string_values.size();
      for(unsigned i = 0; i < num_values; ++i){
        _vertices.push_back((float)atof(string_values[i]));
      }
      string_values.clear();
      break;
    case INDEX_CHARACTER:
      split_string(&string_values, line + DATA_OFFSET);
      num_values = string_values.size();
      for (unsigned i = 0; i < num_values; ++i) {
        _indicies.push_back((unsigned)atoi(string_values[i]));
      }
      string_values.clear();
      break;
    }
  }
}