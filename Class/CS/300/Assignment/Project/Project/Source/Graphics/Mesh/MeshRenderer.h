/* All content(c) 2017 DigiPen(USA) Corporation, all rights reserved. */
#pragma once

#include <unordered_set>
#include <GL\glew.h>
 
#include "../../Math/Matrix4.h"
#include "../Shader/ShaderLibrary.h"
#include "../Color.h"
#include "Mesh.h"

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
      GLuint vbo_fn, GLuint vao_fn, unsigned int vertices_fn,
      GLuint vbo_t,  GLuint vao_t,  unsigned int vertices_t,
      GLuint vbo_bt, GLuint vao_bt, unsigned int vertices_bt): 
      _vbo(vbo), _ebo(ebo), _vao(vao), _elements(elements),
      _vboVertexNormal(vbo_vn), _vaoVertexNormal(vao_vn),
      _vertexNormalVertexCount(vertices_vn),
      _vboFaceNormal(vbo_fn), _vaoFaceNormal(vao_fn),
      _faceNormalVertexCount(vertices_fn),
      _vboTangent(vbo_t), _vaoTangent(vao_t),
      _tangentVertexCount(vertices_t),
      _vboBitangent(vbo_bt), _vaoBitangent(vao_bt),
      _bitangentVertexCount(vertices_bt),
      _showVertexNormals(false), _showFaceNormals(false), 
      _showTangents(false), _showBitangents(false),
      _showWireframe(false),
      _color(1.0f, 1.0f, 1.0f),
      _vertexNormalColor(0.0f, 0.0f, 0.0f), _faceNormalColor(0.0f, 0.0f, 0.0f),
      _tangentColor(0.0f, 0.0f, 0.0f), _bitangentColor(0.0f, 0.0f, 0.0f)
      {}
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

    // Tangent line buffer info
    GLuint _vboTangent;
    GLuint _vaoTangent;
    unsigned int _tangentVertexCount;

    // Bitangent line buffer info
    GLuint _vboBitangent;
    GLuint _vaoBitangent;
    unsigned int _bitangentVertexCount;

    //! Determines whether the vertex normals should be displayed
    bool _showVertexNormals;
    //! Determines whether the face normals should be displayed
    bool _showFaceNormals;

    bool _showTangents;
    bool _showBitangents;
    //! Determines whether the model is displayed in wireframe mode or not
    bool _showWireframe;
    //! The color the main mesh will be rendered with
    Color _color;
    //! The color of the vertex normals
    Color _vertexNormalColor;
    //! The color of the face normals
    Color _faceNormalColor;

    Color _tangentColor;
    Color _bitangentColor;
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
  static MeshObject * Upload(Mesh * mesh);
  static void Unload(MeshObject * mesh_object);
  static void Render(MeshObject * mesh_object, ShaderType shader_type,
    const Math::Matrix4 & projection, const Math::Matrix4 & view, 
    const Math::Matrix4 & model);
  static void ReloadShader(ShaderType shader_type);
  static SolidShader * GetSolidShader();
  static PhongShader * GetPhongShader();
  static GouraudShader * GetGouraudShader();
  static BlinnShader * GetBlinnShader();
  static LineShader * GetLineShader();
  static int ShaderTypeToInt(ShaderType shader_type);
  static ShaderType IntToShaderType(int shader_int);
private:
  //! The vector of currently loaded Mesh objects
  static std::unordered_set<MeshObject *> _meshObjects;
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