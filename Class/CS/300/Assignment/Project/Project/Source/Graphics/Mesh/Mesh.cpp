/* All content(c) 2017 DigiPen(USA) Corporation, all rights reserved. */
/*****************************************************************************/
/*!
\file Mesh.cpp
\author Connor Deakin
\par E-mail: connor.deakin\@digipen.edu
\par Project: CS 300
\date 02/10/2017
\brief
  Contains the implementation of the mesh class.
*/
/*****************************************************************************/

#include <cstring>
#include "../../Utility/Error.h"

#include "Mesh.h"
#include "../../Math/Vector3.h"

#define PI  3.14159265359f
#define PI2 6.28318530718f

#define VERTS_PER_LINE 2
#define FACE_NUMELEMENTS 3
#define READ_BUFFERSIZE 100
#define DATA_OFFSET 2
#define NULL_TERMINATOR '\0'
#define VERTEX_CHARACTER 'v'
#define INDEX_CHARACTER 'f'
#define VALUE_DELIMETER ' '

Mesh::Mesh(const std::string & file_name, FileType type, int mapping_type)
{
  switch (type){
  case OBJ:
    LoadObj(file_name);
    break;
  default:
    Error error("Mesh.cpp", "Mesh Constructor");
    error.Add("The Mesh class cannot load this file type");
    throw(error);
  }
  // perform mapping if necessary
  switch (mapping_type)
  {
  case MESH_MAPPING_SPHERICAL:
    PerformSphericalMapping();
    break;
  case MESH_MAPPING_CYLINDRICAL:
    PerformCylindricalMapping();
    break;
  case MESH_MAPPING_PLANAR:
    PerformPlanarMapping();
    break;
  default:
    break;
  }

  // Zero this shit out for now
  for (Vertex & vert : _vertices) {
    vert.tx = 0.0f;
    vert.ty = 0.0f;
    vert.tz = 0.0f;

    vert.bx = 0.0f;
    vert.by = 0.0f;
    vert.bz = 0.0f;
  }
}

Mesh * Mesh::Load(const std::string & file_name, FileType type, 
  int mapping_type)
{
  return new Mesh(file_name, type, mapping_type);
}

void Mesh::Purge(Mesh * mesh)
{
  delete mesh;
}

void Mesh::SetNormalLineLengthMeshRelative(float new_length)
{
  float scale_factor = new_length / _normalLineMagnitude;
  unsigned num_norms = _vertexNormalLines.size();
  for(Line & line : _vertexNormalLines)
    ScaleLine(line, scale_factor);
  for(Line & line : _faceNormalLines)
    ScaleLine(line, scale_factor);
  _normalLineMagnitude = new_length;
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

unsigned Mesh::VertexNormalLineSizeVertices()
{
  return _vertexNormalLines.size() * VERTS_PER_LINE;
}

void * Mesh::FaceNormalLineData()
{
  return (void *)_faceNormalLines.data();
}

unsigned Mesh::FaceNormalLineSizeBytes()
{
  return _faceNormalLines.size() * sizeof(Line);
}

unsigned Mesh::FaceNormalLineSizeVertices()
{
  return _faceNormalLines.size() * VERTS_PER_LINE;
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
    // ignore vt and vn indicies
    if (string[i] == '/')
      string[i] = NULL_TERMINATOR;
    ++i;
  }
}

inline void Mesh::PerformSphericalMapping()
{
  for (Vertex & vert : _vertices) {
    float theta = Math::ArcTan2(vert.px, vert.pz);
    float phi = Math::ArcCos(vert.py);
    vert.u = (theta + PI) / PI2;
    vert.v = phi / PI;
  }
}

inline void Mesh::PerformCylindricalMapping()
{
  for (Vertex & vert : _vertices) {
    float theta = Math::ArcTan2(vert.px, vert.pz);
    vert.u = (theta + PI) / PI2;
    vert.v = (vert.py + 1.0f) / 2.0f;
  }
}

inline void Mesh::PerformPlanarMapping()
{
  for (Vertex & vert : _vertices) {
    // model position
    Math::Vector3 mp(vert.px, vert.py, vert.pz);
    Math::Vector3 mpa(Math::Abs(mp.x), Math::Abs(mp.y), Math::Abs(mp.z));

    if (mpa.x > mpa.y && mpa.x > mpa.z) {
      vert.u = (mp.z / mp.x + 1.0) / 2.0;
      vert.v = (mp.y / mp.x + 1.0) / 2.0;
    }
    // Y mapping
    else if (mpa.y > mpa.x && mpa.y > mpa.z) {
      vert.u = (mp.x / mp.y + 1.0) / 2.0;
      vert.v = (mp.z / mp.y + 1.0) / 2.0;
    }
    // Z mapping
    else if (mpa.z > mpa.x && mpa.z > mpa.y) {
      vert.u = (mp.x / mp.z + 1.0) / 2.0;
      vert.v = (mp.y / mp.z + 1.0) / 2.0;
    }
  }
}

inline void Mesh::CalculateFaceNormals()
{
  unsigned num_faces = _faces.size();
  for (unsigned i = 0; i < num_faces; ++i) {
    Face & face = _faces[i];
    Math::Vector3 ab;
    ab.x = _vertices[face.b].px - _vertices[face.a].px;
    ab.y = _vertices[face.b].py - _vertices[face.a].py;
    ab.z = _vertices[face.b].pz - _vertices[face.a].pz;
    Math::Vector3 ac;
    ac.x = _vertices[face.c].px - _vertices[face.a].px;
    ac.y = _vertices[face.c].py - _vertices[face.a].py;
    ac.z = _vertices[face.c].pz - _vertices[face.a].pz;
    Math::Vector3 result = Math::Cross(ab, ac);
    float length = result.Length();
    _showFaceNormals.push_back(result / length);
  }
}

inline void Mesh::CalculateVertexNormals(std::vector<std::vector<unsigned> > * adjacencies)
{
  unsigned num_vertices = adjacencies->size();
  for (unsigned vert = 0; vert < num_vertices; ++vert) {
    std::vector<unsigned> & vert_adjacencies = (*adjacencies)[vert];
    unsigned num_adjacencies = (*adjacencies)[vert].size();
    
    Math::Vector3 normal_sum(0.0f, 0.0f, 0.0f);
    unsigned num_normals = 0;
    while(vert_adjacencies.size() > 0) {
      if(RemoveParallelFace(vert_adjacencies))
        continue;
      unsigned face_index = vert_adjacencies.back();
      normal_sum += _showFaceNormals[face_index];
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
  const Math::Vector3 & search_normal = _showFaceNormals[vert_adjacencies.back()];
  unsigned num_adjacencies = vert_adjacencies.size();
  for (unsigned i = 0; i < num_adjacencies - 1; ++i) {
    unsigned face_index = vert_adjacencies[i];
    const Math::Vector3 & compare_normal = _showFaceNormals[face_index];
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
    throw(error);
  }
  // TODO - Reserve space in vectors
  // read in vertex and index data
  while (!file_stream.eof()){
    char line[READ_BUFFERSIZE];
    file_stream.getline(line, READ_BUFFERSIZE);
    std::vector<char *> string_values;
    unsigned num_values;
    unsigned data_offset = 1;
    switch (line[0]) {
    case VERTEX_CHARACTER:
      // temporary
      if(line[1] != ' ')
        continue;
      while(line[data_offset] == VALUE_DELIMETER)
        ++data_offset;
      // line contains vertex data
      split_string(&string_values, line + data_offset);
      num_values = string_values.size();
      _vertices.push_back(Vertex());
      for(unsigned i = 0; i < num_values; ++i){
        _vertices.back().value[i] = (float)atof(string_values[i]);
      }
      string_values.clear();
      break;
    case INDEX_CHARACTER:
      while (line[data_offset] == VALUE_DELIMETER)
        ++data_offset;
      // line contains index data
      split_string(&string_values, line + data_offset);
      // reading in all indicies
      std::vector<unsigned> index;
      for (char * string_value : string_values)
        index.push_back((unsigned)atoi(string_value) - 1);
      // adding faces
      // line contatining (1 2 4 3) results in faces
      // (1 2 4) && (1 4 3)
      unsigned tri = 0;
      unsigned num_triangles = index.size() - 2;
      for (unsigned i = 0; i < num_triangles; ++i){
        _faces.push_back(Face(index[0], index[1 + tri], index[2 + tri]));
        ++tri;
      }
      string_values.clear();
      break;
    }
  }
  file_stream.close();
  // centering vertices
  Math::Vector3 center(0.0f, 0.0f, 0.0f);
  for (const Vertex & vert : _vertices) {
    center.x += vert.px;
    center.y += vert.py;
    center.z += vert.pz;
  }
  float inverse_denominator = 1.0f / static_cast<float>(_vertices.size());
  center *= inverse_denominator;
  for (Vertex & vert : _vertices) {
    vert.px -= center.x;
    vert.py -= center.y;
    vert.pz -= center.z;
  }
  // normalizing vertices
  // finding the vertex furthest away
  float max_length = 0;
  for (const Vertex & vert : _vertices) {
    float length = vert.px * vert.px + vert.py * vert.py + vert.pz * vert.pz;
    if(length > max_length)
      max_length = length;
  }
  // scale vertex to a unit vertex and scale all other vertices by the
  // same scalar
  max_length = Math::Sqrt(max_length);
  float scale = 1.0f / max_length;
  for (Vertex & vert : _vertices) {
    vert.px *= scale;
    vert.py *= scale;
    vert.pz *= scale;
  }

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
    cur_vnormal.sx = cur_vertex.px;
    cur_vnormal.sy = cur_vertex.py;
    cur_vnormal.sz = cur_vertex.pz;
    // ending point of the vertex normal line
    cur_vnormal.ex = cur_vertex.px + cur_vertex.nx;
    cur_vnormal.ey = cur_vertex.py + cur_vertex.ny;
    cur_vnormal.ez = cur_vertex.pz + cur_vertex.nz;
  }

  // create face normal lines
  unsigned num_faces =  _faces.size();
  _faceNormalLines.resize(num_faces);
  for (unsigned face = 0; face < num_faces; ++face) {
    const Face & cur_face = _faces[face];
    const Math::Vector3 & cur_face_normal = _showFaceNormals[face];
    Line & cur_line = _faceNormalLines[face];
    // the vertices that make up the face
    const Vertex & va = _vertices[cur_face.a];
    const Vertex & vb = _vertices[cur_face.b];
    const Vertex & vc = _vertices[cur_face.c];
    // finding starting point of face normals
    cur_line.sx = (va.px + vb.px + vc.px) / 3.0f;
    cur_line.sy = (va.py + vb.py + vc.py) / 3.0f;
    cur_line.sz = (va.pz + vb.pz + vc.pz) / 3.0f;
    // find the ending points of the face normals
    cur_line.ex = cur_line.sx + cur_face_normal.x;
    cur_line.ey = cur_line.sy + cur_face_normal.y;
    cur_line.ez = cur_line.sz + cur_face_normal.z;
  }
  _normalLineMagnitude = 1.0f;
}

inline void Mesh::ScaleLine(Line & line, float scale)
{
  // current deltas
  float dx = line.ex - line.sx;
  float dy = line.ey - line.sy;
  float dz = line.ez - line.sz;
  // new deltas
  dx *= scale;
  dy *= scale;
  dz *= scale;
  // finding new line end points
  line.ex = line.sx + dx;
  line.ey = line.sy + dy;
  line.ez = line.sz + dz;
}