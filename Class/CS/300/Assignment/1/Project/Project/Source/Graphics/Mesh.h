#pragma once

#include <string>
#include <vector>

class Mesh
{
public:
  enum FileType
  {
    OBJ,
    NUM_FILETYPE
  };
public:
  Mesh(const std::string & file_name, FileType type);
  void * VertexData();
  void * IndexData();
private:
  void LoadObj(const std::string & file_name);
  std::vector<float> _vertices;
  std::vector<unsigned> _indicies;

};