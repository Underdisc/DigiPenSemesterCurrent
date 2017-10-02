#pragma once

#include <string>
#include <vector>

class Mesh
{
private:
  struct Vertex
  {
    Vertex() {}
    Vertex(float x, float y, float z) : x(x), y(y), z(z) {}
    bool Near(const Vertex & other);
    Vertex & operator+=(const Vertex & other);
    Vertex & operator*=(float value);
    union {
      struct {
        float x, y, z;
        float nx, nz, ny;
      };
      float value[6];
    };
  };
  bool Near(const Vertex & lhs, const Vertex & rhs);
  struct Face
  {
    Face() {}
    union {
      struct {
        unsigned a, b, c;
      };
      unsigned index[3];
    };
  };
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
  unsigned IndexDataSize();
  unsigned VertexDataSizeBytes();
  unsigned IndexDataSizeBytes();
private:
  void CalculateFaceNormals();
  void CalculateVertexNormals(std::vector<std::vector<unsigned> > * adjacencies);
  bool RemoveParallelFace(std::vector<unsigned> & vert_adjacencies);
  void CreateAdjacencyList(std::vector<std::vector<unsigned> > * adjacencies);
  void LoadObj(const std::string & file_name);
  std::vector<Vertex> _vertices;
  std::vector<Face> _faces;
  std::vector<Vertex> _vertexNormals;
  std::vector<Vertex> _faceNormals;

};