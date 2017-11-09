/* All content(c) 2017 DigiPen(USA) Corporation, all rights reserved. */
#pragma once

#include <vector>
#include <GL\glew.h>
 
#include "../../Math/Matrix4.h"
#include "../ShaderLibrary.h"
#include "Mesh.h"

/*****************************************************************************/
/*!
\class Color
\brief
Used for storing rgb color values.
*/
/*****************************************************************************/
struct Color
{
  Color() : _x(0.0f), _y(0.0f), _z(0.0f) {}
  Color(float x, float y, float z) : _x(x), _y(y), _z(z) {}
  Color(const Color & other) : _x(other._x), _y(other._y), _z(other._z) {}
  // The rgb value of the color
  union {
    float _values[3];
    struct {
      float _x, _y, _z;
    };
    struct {
      float _r, _g, _b;
    };
  };
};

/*****************************************************************************/
/*!
\class MeshRenderer
\brief
  Used for rendering a mesh, the vertex normals of the mesh, and the face
  normals of the mesh.
*/
/*****************************************************************************/
class MeshRenderer
{
public:
  /***************************************************************************/
  /*!
  \class MeshObject
  \brief
    Contains all of the identifiers for the OpenGL buffers associated with
    a mesh. This also contains bools that represent how the mesh will
    be displayed.
  */
  /***************************************************************************/
  struct MeshObject
  {
    MeshObject(GLuint vbo, GLuint ebo, GLuint vao, unsigned int elements,
      GLuint vbo_vn, GLuint vao_vn, unsigned int vertices_vn,
      GLuint vbo_fn, GLuint vao_fn, unsigned int vertices_fn): 
      _ID(MeshRenderer::_meshObjectsAdded), 
      _vbo(vbo), _ebo(ebo), _vao(vao), _elements(elements),
      _vboVertexNormal(vbo_vn), _vaoVertexNormal(vao_vn),
      _vertexNormalVertexCount(vertices_vn),
      _vboFaceNormal(vbo_fn), _vaoFaceNormal(vao_fn),
      _faceNormalVertexCount(vertices_fn),
      _showVertexNormals(false), _showFaceNormals(false), _showWireframe(false),
      _color(1.0f, 1.0f, 1.0f),
      _vertexNormalColor(0.0f, 0.0f, 0.0f), _faceNormalColor(0.0f, 0.0f, 0.0f)
      {}
    //! The ID for the mesh object
    unsigned int _ID;
    //! VBO for the mesh
    GLuint _vbo;
    //! EBO for the mesh
    GLuint _ebo;
    //! VAO forn the mesh
    GLuint _vao;
    //! The number of elements in the EBO
    unsigned int _elements;
    //! The VBO for the vertex normal lines
    GLuint _vboVertexNormal;
    //! The VAO for the vertex normal lines
    GLuint _vaoVertexNormal;
    //! The number of vertices in the VertexNormal VBO
    unsigned int _vertexNormalVertexCount;
    //! The VBO for the face normal lines
    GLuint _vboFaceNormal;
    //! The VAO for the face normal lines
    GLuint _vaoFaceNormal;
    //! The number of vertices in the FaceNormal VBO
    unsigned int _faceNormalVertexCount;
    //! Determines whether the vertex normals should be displayed
    bool _showVertexNormals;
    //! Determines whether the face normals should be displayed
    bool _showFaceNormals;
    //! Determines whether the model is displayed in wireframe mode or not
    bool _showWireframe;
    //! The color the main mesh will be rendered with
    Color _color;
    //! The color of the vertex normals
    Color _vertexNormalColor;
    //! The color of the face normals
    Color _faceNormalColor;
  };
public:
  enum ShaderType
  {
    PHONG,
    GOURAUD,
    BLINN,
    SOLID,
    NUMSHADERTYPES
  };
public:
  static Color _emissiveColor;
  static Color _globalAmbientColor;
  static Color _fogColor;
  static float _fogNear;
  static float _fogFar;
  static float _nearPlane;
  static float _farPlane;
public:
  static void Initialize();
  static void Purge();
  static unsigned int Upload(Mesh * mesh);
  static void Purge(unsigned int mesh_id);
  static void Render(unsigned int mesh_id, ShaderType shader_type,
    const Math::Matrix4 & projection, const Math::Matrix4 & view, 
    const Math::Matrix4 & model);
  static void ReloadShader(ShaderType shader_type);
  static void ReloadPhong();
  static void ReloadGouraud();
  static void ReloadBlinn();
  static MeshObject * GetMeshObject(unsigned int mesh_id);
  static SolidShader * GetSolidShader();
  static PhongShader * GetPhongShader();
  static GouraudShader * GetGouraudShader();
  static BlinnShader * GetBlinnShader();
  static LineShader * GetLineShader();
  static int ShaderTypeToInt(ShaderType shader_type);
  static ShaderType IntToShaderType(int shader_int);
private:
  static unsigned int MeshIDToIndex(unsigned int mesh_id);
  //! The vector of currently loaded Mesh objects
  static std::vector<MeshObject> _meshObjects;
  //! The number of mesh objects that have been added to the MeshRenderer
  static unsigned int _meshObjectsAdded;
  //! The shader used for drawing vertex and face normals
  static LineShader * _lineShader;
  //! The shader used for drawing single color meshes
  static SolidShader * _solidShader;
  //! The shader used for Phong
  static PhongShader * _phongShader;
  //! The shader used for Gouraud
  static GouraudShader * _gouraudShader;
  //! The shader used for Blinn
  static BlinnShader * _blinnShader;
};