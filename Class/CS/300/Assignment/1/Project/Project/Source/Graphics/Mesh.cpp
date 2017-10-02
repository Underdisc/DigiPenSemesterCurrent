#include <cstring>
#include "../Utility/Error.h"

#include "Mesh.h"
#include "../Math/Vector3.h"

#define EPSILON 1.0e-4f

#define FACE_NUMELEMENTS 3
#define READ_BUFFERSIZE 50
#define DATA_OFFSET 2
#define NULL_TERMINATOR '\0'
#define VERTEX_CHARACTER 'v'
#define INDEX_CHARACTER 'f'
#define VALUE_DELIMETER ' '

Mesh::Vertex & Mesh::Vertex::operator+=(const Vertex & rhs)
{
  x += rhs.x;
  y += rhs.y;
  z += + rhs.z;
  return *this;
}

Mesh::Vertex & Mesh::Vertex::operator*=(float value)
{
  x *= value;
  y *= value;
  z *= value;
  return * this;
}

bool Mesh::Near(const Vertex & lhs, const Vertex & rhs)
{
  float x_diff = lhs.x - rhs.x;
  float y_diff = lhs.y - rhs.y;
  float z_diff = lhs.z - rhs.z;
  if (x_diff > EPSILON || x_diff < -EPSILON)
    return false;
  if (y_diff > EPSILON || y_diff < -EPSILON)
    return false;
  if (z_diff > EPSILON || z_diff < -EPSILON)
    return false;
  return true;
}

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
  std::vector<std::vector<unsigned> > adjacencies;
  CreateAdjacencyList(&adjacencies);
  CalculateFaceNormals();
  CalculateVertexNormals(&adjacencies);
}

void * Mesh::VertexData()
{
  return (void *)_vertices.data();
}

void * Mesh::IndexData()
{
  return (void *)_faces.data();
}

unsigned Mesh::IndexDataSize()
{
  return _faces.size() * FACE_NUMELEMENTS;
}

unsigned Mesh::VertexDataSizeBytes()
{
  return _vertices.size() * sizeof(Vertex);
}

unsigned Mesh::IndexDataSizeBytes()
{
  return _faces.size() * sizeof(Face);
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

inline void Mesh::CalculateFaceNormals()
{
  unsigned num_faces = _faces.size();
  for (unsigned i = 0; i < num_faces; ++i) {
    Face & face = _faces[i];
    Math::Vector3 ab;
    ab.x = _vertices[face.b].x - _vertices[face.a].x;
    ab.y = _vertices[face.b].y - _vertices[face.a].y;
    ab.z = _vertices[face.b].z - _vertices[face.a].z;
    Math::Vector3 ac;
    ac.x = _vertices[face.c].x - _vertices[face.a].x;
    ac.y = _vertices[face.c].y - _vertices[face.a].y;
    ac.z = _vertices[face.c].z - _vertices[face.a].z;
    Math::Vector3 result = Math::Cross(ab, ac);
    result.Normalize();
    _faceNormals.push_back(Vertex(result.x, result.y, result.z));
  }
}

inline void Mesh::CalculateVertexNormals(std::vector<std::vector<unsigned> > * adjacencies)
{
  unsigned num_vertices = adjacencies->size();
  for (int vert = 0; vert < num_vertices; ++vert) {
    std::vector<unsigned> & vert_adjacencies = (*adjacencies)[vert];
    unsigned num_adjacencies = (*adjacencies)[vert].size();
    
    Vertex normal_sum(0.0f, 0.0f, 0.0f);
    unsigned num_normals = 0;
    while(vert_adjacencies.size() > 0) {
      if(RemoveParallelFace(vert_adjacencies))
        continue;
      unsigned face_index = vert_adjacencies.back();
      normal_sum += _faceNormals[face_index];
      ++num_normals;
      vert_adjacencies.pop_back();
    }
    normal_sum *= (1.0f / (float)num_normals);
    _vertices[vert].nx = normal_sum.x;
    _vertices[vert].ny = normal_sum.y;
    _vertices[vert].nz = normal_sum.z;
  }
}

bool Mesh::RemoveParallelFace(std::vector<unsigned> & vert_adjacencies)
{
  Vertex & search_normal = _faceNormals[vert_adjacencies.back()];
  unsigned num_adjacencies = vert_adjacencies.size();
  for (int i = 0; i < num_adjacencies - 1; ++i) {
    unsigned face_index = vert_adjacencies[i];
    Vertex & compare_normal = _faceNormals[face_index];
    if(Near(search_normal, compare_normal)){
      vert_adjacencies.pop_back();
      return true;
    }
  }
  return false;
}

inline void Mesh::CreateAdjacencyList(std::vector<std::vector<unsigned> > * adjacencies)
{
  adjacencies->resize(_vertices.size());
  unsigned num_faces = _faces.size();
  for (unsigned i = 0; i < num_faces; ++i) {
    Face & face = _faces[i];
    (*adjacencies)[face.a].push_back(i);
    (*adjacencies)[face.b].push_back(i);
    (*adjacencies)[face.c].push_back(i);
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
      _vertices.push_back(Vertex());
      for(unsigned i = 0; i < num_values; ++i){
        _vertices.back().value[i] = (float)atof(string_values[i]);
      }
      string_values.clear();
      break;
    case INDEX_CHARACTER:
      split_string(&string_values, line + DATA_OFFSET);
      num_values = string_values.size();
      _faces.push_back(Face());
      for (unsigned i = 0; i < num_values; ++i) {
        _faces.back().index[i] = (unsigned)atoi(string_values[i]) - 1;
      }
      string_values.clear();
      break;
    }
  }
}