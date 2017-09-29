#include "Mesh.h"
#include "../Utility/Error.h"

#define READ_BUFFERSIZE 50
#define VERTEX_CHARACTER 'v'
#define INDEX_CHARACTER 'f'

Mesh::Mesh(const std::string & file_name, FileType type)
{
  switch (type){
  case OBJ:
    LoadObj(file_name);
  default:
    Error error("Mesh.cpp", "Mesh Constructor");
    error.Add("The Mesh class cannot load this file type");
    ErrorLog::Write(error);
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

  while (!file_stream.eof()){
    char line[READ_BUFFERSIZE];
    file_stream.getline(line, READ_BUFFERSIZE);
    switch (line[0]) {
      case VERTEX_CHARACTER:
        
      case INDEX_CHARACTER:
        

    }
    
    
  }
 
}