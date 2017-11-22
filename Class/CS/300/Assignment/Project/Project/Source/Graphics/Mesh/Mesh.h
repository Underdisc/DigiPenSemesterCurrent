/* All content(c) 2017 DigiPen(USA) Corporation, all rights reserved. */
/*****************************************************************************/
/*!
\file Mesh.h
\author Connor Deakin
\par E-mail: connor.deakin\@digipen.edu
\par Project: CS 300
\date 02/10/2017
\brief
  Contains the interface for the Mesh class.
*/
/*****************************************************************************/
#pragma once

#include <string>
#include <vector>

#include "../../Math/Vector3.h"

#define MESH_MAPPING_SPHERICAL 0
#define MESH_MAPPING_CYLINDRICAL 1
#define MESH_MAPPING_PLANAR 2

/*****************************************************************************/
/*!
\class Mesh
\brief
  Can be used to load model file types for vertex and index buffer data.
*/
/*****************************************************************************/
class Mesh
{
public:
  struct Vertex
  {
    Vertex() {}
    union {
      struct {
        float px, py, pz;
        float nx, ny, nz;
        float tx, ty, tz;
        float bx, by, bz;
        float u, v;
      };
      float value[14];
    };
  };
  struct Face
  {
    Face() {}
    Face(unsigned a, unsigned b, unsigned c) : a(a), b(b), c(c) {}
    union {
      struct {
        unsigned a, b, c;
      };
      unsigned index[3];
    };
  };
  struct Line
  {
    Line() {}
    float sx, sy, sz;
    float ex, ey, ez;
  };
public:
  enum FileType
  {
    OBJ,
    NUM_FILETYPE
  };
public:
  Mesh(const std::string & file_name, FileType type, 
    int mapping_type = MESH_MAPPING_SPHERICAL);
  static Mesh * Load(const std::string & file_name, FileType type,
    int mapping_type = MESH_MAPPING_SPHERICAL);
  static void Purge(Mesh * mesh);
  void PerformSphericalMapping();
  void PerformCylindricalMapping();
  void PerformPlanarMapping();
  void SetNormalLineLengthMeshRelative(float new_length);
  unsigned VertexCount();
  unsigned FaceCount();
  void * VertexData();
  void * IndexData();
  unsigned IndexDataSize();
  unsigned VertexDataSizeBytes();
  unsigned IndexDataSizeBytes();
  void * VertexNormalLineData();
  unsigned VertexNormalLineSizeBytes();
  unsigned VertexNormalLineSizeVertices();
  void * FaceNormalLineData();
  unsigned FaceNormalLineSizeBytes();
  unsigned FaceNormalLineSizeVertices();
private:
  void CalculateFaceNormals();
  void CalculateVertexNormals();
  void RemoveParallelAdjacencies(std::vector<unsigned> * adjacencies);
  void CreateVertexAdjacencies();
  void LoadObj(const std::string & file_name);
  void ScaleLine(Line & line, float scale);
  //! The vertices of the mesh.
  std::vector<Vertex> _vertices;
  //! The faces of the mesh.
  std::vector<Face> _faces;
  //! Records the indicies of which faces are adjacent to each vertex.
  std::vector<std::vector<unsigned> > _vertexAdjacencies;
  //! The normals of all faces on the mesh.
  std::vector<Math::Vector3> _faceNormals;

  std::vector<Line> _vertexNormalLines;
  std::vector<Line> _faceNormalLines;
  float _normalLineMagnitude;

};