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

  // calculating normals
  CalculateFaceNormals();
  CreateVertexAdjacencies();
  CalculateVertexNormals();
  // calculating tangents and bitangents
  CalculateFaceTangentsBitangents();
  CalculateVertexTangentsBitangents();

  // create vertex normal, tangent, and bitangent lines
  unsigned num_verts = _vertices.size();
  // allocate enough space for all lines
  _vertexNormalLines.resize(num_verts);
  _vertexTangentLines.resize(num_verts);
  _vertexBitangentLines.resize(num_verts);
  // fill in line data
  for (unsigned i = 0; i < num_verts; ++i) {
    const Vertex & vert = _vertices[i];
    Line & vnormal = _vertexNormalLines[i];
    Line & vtangent = _vertexTangentLines[i];
    Line & vbitangent = _vertexBitangentLines[i];
    // starting and end points of normals
    vnormal.sx = vert.px; vnormal.ex = vert.px + vert.nx;
    vnormal.sy = vert.py; vnormal.ey = vert.py + vert.ny;
    vnormal.sz = vert.pz; vnormal.ez = vert.pz + vert.nz;
    // start and end points of tangents
    vtangent.sx = vert.px; vtangent.ex = vert.px + vert.tx;
    vtangent.sy = vert.py; vtangent.ey = vert.py + vert.ty;
    vtangent.sz = vert.pz; vtangent.ez = vert.pz + vert.tz;
    // state and end points of bitangents
    vbitangent.sx = vert.px; vbitangent.ex = vert.px + vert.bx;
    vbitangent.sy = vert.py; vbitangent.ey = vert.py + vert.by;
    vbitangent.sz = vert.pz; vbitangent.ez = vert.pz + vert.bz;
  }

  // create face normal lines
  unsigned num_faces = _faces.size();
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
    cur_line.sx = (va.px + vb.px + vc.px) / 3.0f;
    cur_line.sy = (va.py + vb.py + vc.py) / 3.0f;
    cur_line.sz = (va.pz + vb.pz + vc.pz) / 3.0f;
    // find the ending points of the face normals
    cur_line.ex = cur_line.sx + cur_face_normal.x;
    cur_line.ey = cur_line.sy + cur_face_normal.y;
    cur_line.ez = cur_line.sz + cur_face_normal.z;
  }
  _normalLineMagnitude = 1.0f;

  // calculate tangents and bitangents
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
{ return _faces.size() * FACE_NUMELEMENTS; }
unsigned Mesh::VertexDataSizeBytes()
{ return _vertices.size() * sizeof(Vertex); }
unsigned Mesh::IndexDataSizeBytes()
{ return _faces.size() * sizeof(Face); }

// Vertex normal line buffer data
void * Mesh::VertexNormalLineData()
{ return (void *)_vertexNormalLines.data(); }
unsigned Mesh::VertexNormalLineSizeBytes()
{ return _vertexNormalLines.size() * sizeof(Line); }
unsigned Mesh::VertexNormalLineSizeVertices()
{ return _vertexNormalLines.size() * VERTS_PER_LINE; }

// Face normal line buffer data
void * Mesh::FaceNormalLineData()
{ return (void *)_faceNormalLines.data(); }
unsigned Mesh::FaceNormalLineSizeBytes()
{ return _faceNormalLines.size() * sizeof(Line); }
unsigned Mesh::FaceNormalLineSizeVertices()
{ return _faceNormalLines.size() * VERTS_PER_LINE; }

// Tangent buffer line information
void * Mesh::TangentLineData()
{ return (void *)_vertexTangentLines.data(); }
unsigned Mesh::TangentLineSizeBytes()
{ return _vertexTangentLines.size() * sizeof(Line); }
unsigned Mesh::TangentLineSizeVertices()
{ return _vertexTangentLines.size() * VERTS_PER_LINE; }

// Bitangent buffer line information
void * Mesh::BitangentLineData()
{ return (void *)_vertexBitangentLines.data(); }
unsigned Mesh::BitangentLineSizeBytes()
{ return _vertexBitangentLines.size() * sizeof(Line); }
unsigned Mesh::BitangentLineSizeVertices()
{ return _vertexBitangentLines.size() * VERTS_PER_LINE; }

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
    // X mapping
    if (mpa.x > mpa.y && mpa.x > mpa.z) {
      vert.u = (mp.z / mp.x + 1.0f) / 2.0f;
      vert.v = (mp.y / mp.x + 1.0f) / 2.0f;
    }
    // Y mapping
    else if (mpa.y > mpa.x && mpa.y > mpa.z) {
      vert.u = (mp.x / mp.y + 1.0f) / 2.0f;
      vert.v = (mp.z / mp.y + 1.0f) / 2.0f;
    }
    // Z mapping
    else if (mpa.z > mpa.x && mpa.z > mpa.y) {
      vert.u = (mp.x / mp.z + 1.0f) / 2.0f;
      vert.v = (mp.y / mp.z + 1.0f) / 2.0f;
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
    _faceNormals.push_back(result / length);
  }
}

inline void Mesh::CalculateVertexNormals()
{
  unsigned num_vertices = _vertices.size();
  for (unsigned i = 0; i < num_vertices; ++i) {
    std::vector<unsigned> & vert_adjacencies = _vertexAdjacencies[i];
    Math::Vector3 normal_sum(0.0f, 0.0f, 0.0f);
    for (unsigned face_index : vert_adjacencies)
      normal_sum += _faceNormals[face_index];
    normal_sum *= (1.0f / (float)vert_adjacencies.size());
    float x2 = normal_sum.x * normal_sum.x;
    float y2 = normal_sum.y * normal_sum.y;
    float z2 = normal_sum.z * normal_sum.z;
    float normal_length = sqrt(x2 + y2 + z2);
    _vertices[i].nx = normal_sum.x / normal_length;
    _vertices[i].ny = normal_sum.y / normal_length;
    _vertices[i].nz = normal_sum.z / normal_length;
  }
}

inline void Mesh::CalculateFaceTangentsBitangents()
{
  unsigned num_faces = _faces.size();
  for (unsigned i = 0; i < num_faces; ++i) {
    const Face & face = _faces[i];
    // getting the verts on the face
    const Vertex & a = _vertices[face.a];
    const Vertex & b = _vertices[face.b];
    const Vertex & c = _vertices[face.c];
    // vector from a to b
    Math::Vector3 P(b.px - a.px, b.py - a.py, b.pz - a.pz);
    // vector from a to c
    Math::Vector3 Q(c.px - a.px, c.py - a.py, c.pz - a.pz);
    // change in uv from a to b
    Math::Vector2 duvP(b.u - a.u, b.v - a.u);
    // change in uv from a to c
    Math::Vector2 duvQ(c.u - a.u, c.v - a.u);
    // inverse determinant
    float id = 1.0f / (duvP.x * duvQ.y - duvQ.x - duvP.y);
    // calculating tangent
    Math::Vector3 tangent;
    tangent.x = id * (duvQ.y * P.x - duvP.y * Q.x);
    tangent.y = id * (duvQ.y * P.y - duvP.y * Q.y);
    tangent.z = id * (duvQ.y * P.z - duvP.y * Q.z);
    // calculating bitangent
    Math::Vector3 bitangent;
    bitangent.x = id * (-duvQ.x * P.x + duvP.x * Q.x);
    bitangent.y = id * (-duvQ.x * P.y + duvP.x * Q.y);
    bitangent.z = id * (-duvQ.x * P.z + duvP.x * Q.z);
    // adding tangent and bitangent vectors
    _faceTangents.push_back(tangent);
    _faceBitangents.push_back(bitangent);
  }
}

inline void Mesh::CalculateVertexTangentsBitangents()
{
  unsigned num_vertices = _vertices.size();
  for (unsigned i = 0; i < num_vertices; ++i) {
    std::vector<unsigned> & adjacencies = _vertexAdjacencies[i];
    // vertex tangent and bitangent
    Math::Vector3 tangent(0.0f, 0.0f, 0.0f);
    Math::Vector3 bitangent(0.0f, 0.0f, 0.0f);
    // taking average of tangents / bitangents from surrounding faces
    for (unsigned face_index : adjacencies) {
      tangent += _faceTangents[face_index];
      bitangent += _faceBitangents[face_index];
    }
    tangent *= (1.0f / adjacencies.size());
    bitangent *= (1.0f / adjacencies.size());
    // normalizing results
    tangent.Normalize();
    bitangent.Normalize();
    // setting vertex tangent
    _vertices[i].tx = tangent.x;
    _vertices[i].ty = tangent.y;
    _vertices[i].tz = tangent.z;
    // setting vertex bitangent
    _vertices[i].bx = bitangent.x;
    _vertices[i].by = bitangent.x;
    _vertices[i].bz = bitangent.x;
  }
}


// Calculate face normals must be called before this function
inline void Mesh::CreateVertexAdjacencies()
{
  // add all adjancencies to vertex adjacencies
  _vertexAdjacencies.resize(_vertices.size());
  unsigned num_faces = _faces.size();
  for (unsigned i = 0; i < num_faces; ++i) {
    Face & face = _faces[i];
    (_vertexAdjacencies)[face.a].push_back(i);
    (_vertexAdjacencies)[face.b].push_back(i);
    (_vertexAdjacencies)[face.c].push_back(i);
  }
  // remove parallel faces from the adjacency list
  for (std::vector<unsigned> & adjacencies : _vertexAdjacencies)
    RemoveParallelAdjacencies(&adjacencies);
}

inline void Mesh::RemoveParallelAdjacencies(std::vector<unsigned> * adjacencies)
{
  // stores the indicies of all adjacencies that need to be removed
  std::vector<unsigned> removable_adjacencies;
  // finding all adjacencies that need to be removed
  unsigned num_adjacencies = adjacencies->size();
  for (unsigned i = 0; i < num_adjacencies; ++i) {
    const Math::Vector3 & search_normal = _faceNormals[(*adjacencies)[i]];
    for (unsigned j = i + 1; j < num_adjacencies; ++j) {
      const Math::Vector3 & compare_normal = _faceNormals[(*adjacencies)[j]];
      if (search_normal == compare_normal) {
        removable_adjacencies.push_back(i);
        break;
      }
    }
  }
  // removing all parallel adjecencies
  for (int i = removable_adjacencies.size() - 1; i >= 0; --i) {
    std::vector<unsigned>::iterator r_it = adjacencies->begin();
    adjacencies->erase(r_it + removable_adjacencies[i]);
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