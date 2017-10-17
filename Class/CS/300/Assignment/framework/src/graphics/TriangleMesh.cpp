#include "Precompiled.h"
#include "framework/Debug.h"
#include "graphics/TriangleMesh.h"
#include "graphics/Vertex.h"
#include "math/Math.h"

namespace Graphics
{
  using namespace Math;

  TriangleMesh::Triangle::Triangle(u32 _a, u32 _b, u32 _c)
    : a(_a), b(_b), c(_c)
  {
  }

  TriangleMesh::TriangleMesh()
    : vertices_(), triangles_(), triangleNormals_(), vertexArrayObject_(NULL)
  {
  }

  TriangleMesh::~TriangleMesh()
  {
  }

  void TriangleMesh::AddVertex(f32 x, f32 y, f32 z)
  {
    // push_back(Vertex(Vector3(x, y, z)))
    vertices_.emplace_back(Vector3(x, y, z));
  }

  void TriangleMesh::AddTriangle(u32 a, u32 b, u32 c)
  {
    // push_back(Triangle(a, b, c))
    triangles_.emplace_back(a, b, c);
  }

  void TriangleMesh::Preprocess()
  {
    // various useful steps for preparing this model for rendering; none of
    // these would be done for a game

    // TODO(student): implement computing face and vertex normals; remember,
    // you do not want adjacent faces with the same normal to affect subsequent
    // vertices that are shared by those two faces (otherwise it will bias the
    // vertex normal's direction toward those two faces, which is incorrect)

    centerMesh();
    normalizeVertices();
  }

  int TriangleMesh::GetVertexCount() const
  {
    return vertices_.size();
  }

  int TriangleMesh::GetTriangleCount() const
  {
    return triangles_.size();
  }

  Math::Vector3 TriangleMesh::GetTriangleCentroid(u32 tidx) const
  {
    // average all vertices of the triangle to find the centroid
    Triangle const &tri = triangles_[tidx];
    Vector3 centroid = vertices_[tri.a].vertex;
    centroid += vertices_[tri.b].vertex;
    centroid += vertices_[tri.c].vertex;
    return centroid * (1.f / 3.f);
  }

  Vertex const &TriangleMesh::GetVertex(u32 vidx) const
  {
    Assert(vidx < vertices_.size(), "Error: vertex index out of"
      " bounds: %d", vidx);
    return vertices_[vidx];
  }

  Vertex &TriangleMesh::GetVertex(u32 vidx)
  {
    Assert(vidx < vertices_.size(), "Error: vertex index out of"
      " bounds: %d", vidx);
    return vertices_[vidx];
  }

  TriangleMesh::Triangle const &TriangleMesh::GetTriangle(u32 tidx) const
  {
    Assert(tidx < triangles_.size(), "Error: triangle index out of"
      " bounds: %d", tidx);
    return triangles_[tidx];
  }

  u32 TriangleMesh::GetPolygonIndex(u32 tidx, u8 vertex) const
  {
    Assert(tidx < triangles_.size(), "Error: triangle index out of"
      " bounds: %d", tidx);
    Assert(vertex < 3, "Error: vertex index within triangle out of bounds: %d"
      " (expected 0 <= vertex < 3)", vertex);
    return triangles_[tidx].indices[vertex];
  }

  Vector3 const &TriangleMesh::GetPolygonNormal(u32 tidx) const
  {
    Assert(tidx < triangleNormals_.size(), "Error: triangle normal index"
      " out of bounds: %d", tidx);
    return triangleNormals_[tidx];
  }

  void TriangleMesh::Build(std::shared_ptr<ShaderProgram> program)
  {
    // Construct a new VAO using each of the triangles and vertices stored
    // within this TriangleMesh; this is done in a less efficient way possible.
    // A simple memcpy could actually be used to copy data from the mesh
    // directly into the IBO and VBO.
    auto *array = new VertexArrayObject(vertices_.size(), triangles_.size());
    auto &vbo = array->GetVertexBufferObject();
    auto &ibo = array->GetIndexBufferObject();

    // add all of the vertices to the VBO
    for (size_t i = 0; i < vertices_.size(); ++i)
      vbo.AddVertex(vertices_[i]);

    // add all indices (per triangle) to the IBO
    for (size_t i = 0; i < triangles_.size(); ++i)
    {
      Triangle &tri = triangles_[i];
      ibo.AddTriangle(tri.a, tri.b, tri.c);
    }

    // upload the contents of the VBO and IBO to the GPU and build the VAO
    array->Build(program);

    // wrap the VAO into a shared_ptr and save it locally to be rendered
    vertexArrayObject_ = std::shared_ptr<VertexArrayObject>(array);
  }

  void TriangleMesh::Render()
  {
    // if the VAO has been built for this mesh, bind and render it
    if (vertexArrayObject_)
    {
      vertexArrayObject_->Bind();
      vertexArrayObject_->Render();
      vertexArrayObject_->Unbind();
    }
  }

  /* helper methods */

  void TriangleMesh::centerMesh()
  {
    // find the centroid of the entire mesh (average of all vertices, hoping for
    // no overflow) and translate all vertices by the negative of this centroid
    // to ensure all transformations are about the origin
    Vector3 centroid(0.f);
    for (auto &vert : vertices_)
      centroid += vert.vertex;
    centroid *= 1.f / static_cast<f32>(vertices_.size());
    // translate by negative centroid to center model at (0, 0, 0)
    centroid = -centroid;
    for (auto &vert : vertices_)
      vert.vertex += centroid;
  }

  void TriangleMesh::normalizeVertices()
  {
    // find the extent of this mesh and normalize all vertices by scaling them
    // by the inverse of the smallest value of the extent
    Vector3 minimum = vertices_[0].vertex;
    Vector3 maximum = vertices_[0].vertex;
    for (auto &vert : vertices_)
    {
      f32 x = vert.vertex.x, y = vert.vertex.y, z = vert.vertex.z;
      minimum.x = std::min(minimum.x, x);
      minimum.y = std::min(minimum.y, y);
      minimum.z = std::min(minimum.z, z);
      maximum.x = std::max(maximum.x, x);
      maximum.y = std::max(maximum.y, y);
      maximum.z = std::max(maximum.z, z);
    }
    Vector3 extent = maximum - minimum;
    f32 scalar = 1.f / std::min(std::min(extent.x, extent.y), extent.z);
    for (auto &vert : vertices_)
      vert.vertex *= scalar;
  }
}
