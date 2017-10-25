#include "../../Math/Matrix4.h"

#include "MeshRenderer.h"

// static initializations
Color MeshRenderer::_fogColor(1.0f, 0.0f, 0.0f);
float MeshRenderer::_nearPlane = 0.1f;
float MeshRenderer::_farPlane = 2.0f;
std::vector<MeshRenderer::MeshObject> MeshRenderer::_meshObjects;
PhongShader * MeshRenderer::_phongShader = nullptr;
GouraudShader * MeshRenderer::_gouraudShader = nullptr;
LineShader * MeshRenderer::_lineShader = nullptr;


void MeshRenderer::Initialize()
{ 
  // allocating shaders
  _phongShader = new PhongShader();
  _gouraudShader = new GouraudShader();
  _lineShader = new LineShader();
}

/*****************************************************************************/
/*!
\brief
  Destroys all of the MeshObjects and destroys the shader for the mesh render.
*/
/*****************************************************************************/
void MeshRenderer::Purge()
{
  // deleting all mesh objects
  for (MeshObject & mesh_object : _meshObjects) {
    glDeleteBuffers(1, &mesh_object._vbo);
    glDeleteBuffers(1, &mesh_object._ebo);
    glDeleteVertexArrays(1, &mesh_object._vao);
    // freeing normal line buffers
    glDeleteBuffers(1, &mesh_object._vboVertexNormal);
    glDeleteVertexArrays(1, &mesh_object._vaoVertexNormal);
    glDeleteBuffers(1, &mesh_object._vboFaceNormal);
    glDeleteVertexArrays(1, &mesh_object._vaoFaceNormal);
    _meshObjects.clear();
  }
  // deallocating all shaders
  _phongShader->Purge();
  _gouraudShader->Purge();
  _lineShader->Purge();
  delete _phongShader;
  delete _gouraudShader;
  delete _lineShader;
}

/*****************************************************************************/
/*!
\brief Used to upload a mesh, its vertex normal lines, and its face normal
  lines to the gpu.

\param mesh
  The mesh to be uploaded.

\return The mesh's id within the mesh renderer.
*/
/*****************************************************************************/
unsigned int MeshRenderer::Upload(Mesh * mesh)
{
  // primary mesh upload
  GLuint vbo, ebo, vao;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, mesh->VertexDataSizeBytes(), mesh->VertexData(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->IndexDataSizeBytes(), mesh->IndexData(), GL_STATIC_DRAW);
  // enable all vertex attributes
  _phongShader->EnableAttributes();
  _gouraudShader->EnableAttributes();
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  // vertex Normal upload
  GLuint vbo_vn, vao_vn;
  glGenVertexArrays(1, &vao_vn);
  glGenBuffers(1, &vbo_vn);
  glBindVertexArray(vao_vn);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_vn);
  glBufferData(GL_ARRAY_BUFFER, mesh->VertexNormalLineSizeBytes(), mesh->VertexNormalLineData(), GL_STATIC_DRAW);
  glVertexAttribPointer(_lineShader->APosition, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
  glEnableVertexAttribArray(_lineShader->APosition);
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  // face normal upload 
  GLuint vbo_fn, vao_fn;
  glGenVertexArrays(1, &vao_fn);
  glGenBuffers(1, &vbo_fn);
  glBindVertexArray(vao_fn);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_fn);
  glBufferData(GL_ARRAY_BUFFER, mesh->FaceNormalLineSizeBytes(), mesh->FaceNormalLineData(), GL_STATIC_DRAW);
  glVertexAttribPointer(_lineShader->APosition, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
  glEnableVertexAttribArray(_lineShader->APosition);
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  // adding new mesh object
  _meshObjects.push_back(MeshObject(vbo, ebo, vao, mesh->IndexDataSize(),
    vbo_vn, vao_vn, mesh->VertexNormalLineSizeVertices(),
    vbo_fn, vao_fn, mesh->FaceNormalLineSizeVertices()));
  return _meshObjects.size() - 1;
}

void MeshRenderer::Purge(unsigned int mesh_id)
{
  // grabbing mesh from mesh pool
  MeshObject & mesh_object = _meshObjects[mesh_id];
  // freeing mesh buffers
  glDeleteBuffers(1, &mesh_object._vbo);
  glDeleteBuffers(1, &mesh_object._ebo);
  glDeleteVertexArrays(1, &mesh_object._vao);
  // freeing normal line buffers
  glDeleteBuffers(1, &mesh_object._vboVertexNormal);
  glDeleteVertexArrays(1, &mesh_object._vaoVertexNormal);
  glDeleteBuffers(1, &mesh_object._vboFaceNormal);
  glDeleteVertexArrays(1, &mesh_object._vaoFaceNormal);
  // removing mesh from mesh object vector
  std::vector<MeshObject>::iterator it = _meshObjects.begin();
  it = it + mesh_id;
  _meshObjects.erase(it);
}

void MeshRenderer::Render(unsigned int mesh_id, ShaderType shader_type,
  const Math::Matrix4 & projection,
  const Math::Matrix4 & view, const Math::Matrix4 & model)
{
  // grabbing mesh from mesh pool
  MeshObject & mesh_object = _meshObjects[mesh_id];
  // updating shader
  switch (shader_type)
  {
  // PHONG SHADING 
  case ShaderType::PHONG:
    _phongShader->Use();
    glUniformMatrix4fv(_phongShader->UProjection, 1, GL_TRUE, projection.array);
    glUniformMatrix4fv(_phongShader->UView, 1, GL_TRUE, view.array);
    glUniformMatrix4fv(_phongShader->UModel, 1, GL_TRUE, model.array);
    glUniform3f(_phongShader->UFogColor, 
      _fogColor._r, _fogColor._g, _fogColor._b);
    glUniform1f(_phongShader->UNearPlane, _nearPlane);
    glUniform1f(_phongShader->UFarPlane, _farPlane);
    break;
  // GOURAUD SHADING
  case ShaderType::GOURAUD:
    _gouraudShader->Use();
    glUniformMatrix4fv(_gouraudShader->UProjection, 1, GL_TRUE, projection.array);
    glUniformMatrix4fv(_gouraudShader->UView, 1, GL_TRUE, view.array);
    glUniformMatrix4fv(_gouraudShader->UModel, 1, GL_TRUE, model.array);
    glUniform3f(_gouraudShader->UObjectColor,
      mesh_object._color._r,
      mesh_object._color._g,
      mesh_object._color._b);
    break;
  case ShaderType::BLINN:
    break;
  case ShaderType::TOON:
    break;
  default:
    break;
  }
  // drawing mesh
  glBindVertexArray(mesh_object._vao);
  if (mesh_object._showWireframe)
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  else
    glPolygonMode(GL_FRONT, GL_FILL);
  glDrawElements(GL_TRIANGLES, mesh_object._elements, GL_UNSIGNED_INT, nullptr);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glBindVertexArray(0);

  // drawing normal lines
  _lineShader->Use();
  glUniformMatrix4fv(_lineShader->UProjection, 1, GL_TRUE, projection.array);
  glUniformMatrix4fv(_lineShader->UView, 1, GL_TRUE, view.array);
  glUniformMatrix4fv(_lineShader->UModel, 1, GL_TRUE, model.array);
  if (mesh_object._showVertexNormals) {
    // vertex normals
    glUniform3f(_lineShader->ULineColor,
      mesh_object._vertexNormalColor._r, 
      mesh_object._vertexNormalColor._g,
      mesh_object._vertexNormalColor._b);
    glBindVertexArray(mesh_object._vaoVertexNormal);
    glDrawArrays(GL_LINES, 0, mesh_object._vertexNormalVertexCount);
    glBindVertexArray(0);
  }
  if (mesh_object._showFaceNormals) {
    // face normals
    glUniform3f(_lineShader->ULineColor,
      mesh_object._faceNormalColor._r,
      mesh_object._faceNormalColor._g,
      mesh_object._faceNormalColor._b);
    glBindVertexArray(mesh_object._vaoFaceNormal);
    glDrawArrays(GL_LINES, 0, mesh_object._faceNormalVertexCount);
    glBindVertexArray(0);
  }
}

void MeshRenderer::ReloadShader(ShaderType shader_type)
{
  switch (shader_type)
  {
  case PHONG:
    ReloadPhong();
    break;
  case GOURAUD:
    ReloadGouraud();
    break;
  case BLINN:
    break;
  case TOON:
    break;
  default:
    break;
  }
}

void MeshRenderer::ReloadPhong()
{
  // disabling vertex attributes
  for (MeshObject & mesh_object : _meshObjects) {
    glBindVertexArray(mesh_object._vao);
    _phongShader->DisableAttributes();
    glBindVertexArray(0);
  }
  // creating new shader
  delete _phongShader;
  _phongShader = new PhongShader();
  // enabling vertex attributes
  for (MeshObject & mesh_object : _meshObjects) {
    glBindBuffer(GL_ARRAY_BUFFER, mesh_object._vbo);
    glBindVertexArray(mesh_object._vao);
    _phongShader->EnableAttributes();
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }
}

void MeshRenderer::ReloadGouraud()
{
  // disabling vertex attributes
  for (MeshObject & mesh_object : _meshObjects) {
    glBindVertexArray(mesh_object._vao);
    _gouraudShader->DisableAttributes();
    glBindVertexArray(0);
  }
  // creating new shader
  delete _gouraudShader;
  _gouraudShader = new GouraudShader();
  // enabling vertex attributes
  for (MeshObject & mesh_object : _meshObjects) {
    glBindBuffer(GL_ARRAY_BUFFER, mesh_object._vbo);
    glBindVertexArray(mesh_object._vao);
    _gouraudShader->EnableAttributes();
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }
}

MeshRenderer::MeshObject * MeshRenderer::GetMeshObject(unsigned int mesh_id)
{
  return &(_meshObjects[mesh_id]);
}

PhongShader * MeshRenderer::GetPhongShader()
{
  return _phongShader;
}

GouraudShader * MeshRenderer::GetGouraudShader()
{
  return _gouraudShader;
}

LineShader * MeshRenderer::GetLineShader()
{
  return _lineShader;
}

int MeshRenderer::ShaderTypeToInt(ShaderType shader_type)
{
  switch (shader_type)
  {
  case ShaderType::PHONG:
    return 0;
  case ShaderType::GOURAUD:
    return 1;
  case ShaderType::BLINN:
    return 2;
  case ShaderType::TOON:
    return 3;
  default:
    return ShaderType::NUMSHADERTYPES;
  }
}

MeshRenderer::ShaderType MeshRenderer::IntToShaderType(int shader_int)
{
  switch (shader_int)
  {
  case 0:
    return ShaderType::PHONG;
  case 1:
    return ShaderType::GOURAUD;
  case 2:
    return ShaderType::BLINN;
  case 3:
    return ShaderType::TOON;
  default:
    return ShaderType::NUMSHADERTYPES;
  }
}
