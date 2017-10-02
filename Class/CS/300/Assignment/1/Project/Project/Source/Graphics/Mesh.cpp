#include <cstring>
#include "../Utility/Error.h"

#include "Mesh.h"
#include "../Math/Vector3.h"

#define FACE_NUMELEMENTS 3
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

unsigned Mesh::VertexCount()
{
  return _vertices.size();
}

unsigned Mesh::FaceCount()
{
  return _faces.size();
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

void * Mesh::VertexNormalLineData()
{
  return (void *)_vertexNormalLines.data();
}

unsigned Mesh::VertexNormalLineSizeBytes()
{
  return _vertexNormalLines.size() * sizeof(Line);
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
    float length = result.Length();
    _faceNormals.push_back(result / length);
  }
}

inline void Mesh::CalculateVertexNormals(std::vector<std::vector<unsigned> > * adjacencies)
{
  unsigned num_vertices = adjacencies->size();
  for (int vert = 0; vert < num_vertices; ++vert) {
    std::vector<unsigned> & vert_adjacencies = (*adjacencies)[vert];
    unsigned num_adjacencies = (*adjacencies)[vert].size();
    
    Math::Vector3 normal_sum(0.0f, 0.0f, 0.0f);
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
    float x2 = normal_sum.x * normal_sum.x;
    float y2 = normal_sum.y * normal_sum.y;
    float z2 = normal_sum.z * normal_sum.z;
    float normal_length = sqrt(x2 + y2 + z2);
    _vertices[vert].nx = normal_sum.x / normal_length;
    _vertices[vert].ny = normal_sum.y / normal_length;
    _vertices[vert].nz = normal_sum.z / normal_length;
  }
}

bool Mesh::RemoveParallelFace(std::vector<unsigned> & vert_adjacencies)
{
  const Math::Vector3 & search_normal = _faceNormals[vert_adjacencies.back()];
  unsigned num_adjacencies = vert_adjacencies.size();
  for (int i = 0; i < num_adjacencies - 1; ++i) {
    unsigned face_index = vert_adjacencies[i];
    const Math::Vector3 & compare_normal = _faceNormals[face_index];
    if(search_normal == compare_normal){
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
  // open obj file
  std::ifstream file_stream(file_name.c_str());
  if (!file_stream.is_open()) {
    Error error("Mesh.cpp", "LoadObj");
    error.Add("File failed to open.");
    error.Add(file_name.c_str());
    ErrorLog::Write(error);
    return;
  }
  // TODO - Reserve space in vectors
  // read in vertex and index data
  while (!file_stream.eof()){
    char line[READ_BUFFERSIZE];
    file_stream.getline(line, READ_BUFFERSIZE);
    std::vector<char *> string_values;
    unsigned num_values;
    switch (line[0]) {
    case VERTEX_CHARACTER:
      // line contains vertex data
      split_string(&string_values, line + DATA_OFFSET);
      num_values = string_values.size();
      _vertices.push_back(Vertex());
      for(unsigned i = 0; i < num_values; ++i){
        _vertices.back().value[i] = (float)atof(string_values[i]);
      }
      string_values.clear();
      break;
    case INDEX_CHARACTER:
      // line contains index data
      split_string(&string_values, line + DATA_OFFSET);
      num_values = string_values.size();
      _faces.push_back(Face());
      for (unsigned i = 0; i < num_values; ++i) {
        // obj indicies start at 1
        _faces.back().index[i] = (unsigned)atoi(string_values[i]) - 1;
      }
      string_values.clear();
      break;
    }
  }
  file_stream.close();
  // calculating normals
  std::vector<std::vector<unsigned> > adjacencies;
  CreateAdjacencyList(&adjacencies);
  CalculateFaceNormals();
  CalculateVertexNormals(&adjacencies);
  
  // create vertex normal lines
  unsigned num_verts = _vertices.size();
  _vertexNormalLines.resize(num_verts);
  for (unsigned vert = 0; vert < num_verts; ++vert) {
    const Vertex & cur_vertex = _vertices[vert];
    Line & cur_vnormal = _vertexNormalLines[vert];
    // starting point of the vertex normal line
    cur_vnormal.sx = cur_vertex.x;
    cur_vnormal.sy = cur_vertex.y;
    cur_vnormal.sz = cur_vertex.z;
    // ending point of the vertex normal line
    cur_vnormal.ex = cur_vertex.x + cur_vertex.nx;
    cur_vnormal.ey = cur_vertex.y + cur_vertex.ny;
    cur_vnormal.ez = cur_vertex.z + cur_vertex.nz;
  }

  // create face normal lines
  unsigned num_faces =  _faces.size();
  _faceNormalLines.resize(num_faces);
  for (unsigned face = 0; face < num_faces; ++face) {
    const Face & cur_face = _faces[face];
    const Math::Vector3 & cur_face_normal = _faceNormals[face];
    Line & cur_line = _faceNormalLines[face];
    // the vertices that make up the face
    const Vertex & va = _vertices[cur_face.a];
    const Vertex & vb = _vertices[cur_face.b];
    const Vertex & vc = _vertices[cur_face.c];
    // finding starting point of face normals
    cur_line.sx = (va.x + vb.x + vc.x) / 3.0f;
    cur_line.sy = (va.y + vb.y + vc.y) / 3.0f;
    cur_line.sz = (va.z + vb.z + vc.z) / 3.0f;
    // find the ending points of the face normals
    cur_line.ex = cur_line.sx + cur_face_normal.x;
    cur_line.ey = cur_line.sy + cur_face_normal.y;
    cur_line.ez = cur_line.sz + cur_face_normal.z;
  }
}