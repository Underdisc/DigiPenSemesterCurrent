/* All content(c) 2017 DigiPen(USA) Corporation, all rights reserved. */
#include "../../Math/Matrix4.h"
#include "../../Utility/Error.h"

#include "MeshRenderer.h"

// static initializations
Color MeshRenderer::_emissiveColor(0.0f, 0.0f, 0.0f);
Color MeshRenderer::_globalAmbientColor(0.2f, 0.2f, 0.2f);
Color MeshRenderer::_fogColor(0.3f, 0.3f, 0.3f);
float MeshRenderer::_fogNear = 10.0f;
float MeshRenderer::_fogFar = 20.0f;
float MeshRenderer::_nearPlane = 0.1f;
float MeshRenderer::_farPlane = 20.0f;
unsigned int MeshRenderer::_meshObjectsAdded = 0;
std::unordered_set<MeshRenderer::MeshObject *> MeshRenderer::_meshObjects;
LineShader * MeshRenderer::_lineShader = nullptr;
SolidShader * MeshRenderer::_solidShader = nullptr;
PhongShader * MeshRenderer::_phongShader = nullptr;
GouraudShader * MeshRenderer::_gouraudShader = nullptr;
BlinnShader * MeshRenderer::_blinnShader = nullptr;


void MeshRenderer::Initialize()
{ 
  _lineShader = new LineShader();
  _solidShader = new SolidShader();
  _phongShader = new PhongShader();
  _gouraudShader = new GouraudShader();
  _blinnShader = new BlinnShader();
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
  for (MeshObject * mesh_object : _meshObjects) {
    glDeleteBuffers(1, &mesh_object->_vbo);
    glDeleteBuffers(1, &mesh_object->_ebo);
    glDeleteVertexArrays(1, &mesh_object->_vao);
    // freeing normal line buffers
    glDeleteBuffers(1, &mesh_object->_vboVertexNormal);
    glDeleteVertexArrays(1, &mesh_object->_vaoVertexNormal);
    glDeleteBuffers(1, &mesh_object->_vboFaceNormal);
    glDeleteVertexArrays(1, &mesh_object->_vaoFaceNormal);
    delete mesh_object;
  }
  _meshObjects.clear();
  // deallocating all shaders
  _lineShader->Purge();
  _solidShader->Purge();
  _phongShader->Purge();
  _gouraudShader->Purge();
  _blinnShader->Purge();
  delete _lineShader;
  delete _solidShader;
  delete _phongShader;
  delete _gouraudShader;
  delete _blinnShader;
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
MeshRenderer::MeshObject * MeshRenderer::Upload(Mesh * mesh)
{
  // primary mesh upload
  GLuint vbo, ebo, vao;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, mesh->VertexDataSizeBytes(), 
    mesh->VertexData(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->IndexDataSizeBytes(), 
    mesh->IndexData(), GL_STATIC_DRAW);
  // enable all vertex attributes
  _solidShader->EnableAttributes();
  _phongShader->EnableAttributes();
  _gouraudShader->EnableAttributes();
  _blinnShader->EnableAttributes();
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  // vertex Normal upload
  GLuint vbo_vn, vao_vn;
  glGenVertexArrays(1, &vao_vn);
  glGenBuffers(1, &vbo_vn);
  glBindVertexArray(vao_vn);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_vn);
  glBufferData(GL_ARRAY_BUFFER, mesh->VertexNormalLineSizeBytes(), 
    mesh->VertexNormalLineData(), GL_STATIC_DRAW);
  _lineShader->EnableAttributes();
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  // face normal upload 
  GLuint vbo_fn, vao_fn;
  glGenVertexArrays(1, &vao_fn);
  glGenBuffers(1, &vbo_fn);
  glBindVertexArray(vao_fn);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_fn);
  glBufferData(GL_ARRAY_BUFFER, mesh->FaceNormalLineSizeBytes(), 
    mesh->FaceNormalLineData(), GL_STATIC_DRAW);
  _lineShader->EnableAttributes();
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // creating and adding new mesh object
  MeshObject * new_mesh_object = new MeshObject(vbo, ebo, vao, 
    mesh->IndexDataSize(),
    vbo_vn, vao_vn, mesh->VertexNormalLineSizeVertices(),
    vbo_fn, vao_fn, mesh->FaceNormalLineSizeVertices());
  _meshObjects.insert(new_mesh_object);
  _meshObjectsAdded++;
  return new_mesh_object;
}

void MeshRenderer::Unload(MeshObject * mesh_object)
{
  // freeing mesh buffers
  glDeleteBuffers(1, &mesh_object->_vbo);
  glDeleteBuffers(1, &mesh_object->_ebo);
  glDeleteVertexArrays(1, &mesh_object->_vao);
  // freeing normal line buffers
  glDeleteBuffers(1, &mesh_object->_vboVertexNormal);
  glDeleteVertexArrays(1, &mesh_object->_vaoVertexNormal);
  glDeleteBuffers(1, &mesh_object->_vboFaceNormal);
  glDeleteVertexArrays(1, &mesh_object->_vaoFaceNormal);
  // removing mesh from mesh object set and de-allocating
  _meshObjects.erase(mesh_object);
  delete mesh_object;
}

void MeshRenderer::Render(MeshObject * mesh_object, ShaderType shader_type,
  const Math::Matrix4 & projection,
  const Math::Matrix4 & view, const Math::Matrix4 & model)
{
  // updating shader
  switch (shader_type)
  {
  // PHONG SHADING 
  case ShaderType::PHONG:
    _phongShader->Use();
    glUniformMatrix4fv(_phongShader->UProjection, 1, GL_TRUE, projection.array);
    glUniformMatrix4fv(_phongShader->UView, 1, GL_TRUE, view.array);
    glUniformMatrix4fv(_phongShader->UModel, 1, GL_TRUE, model.array);
    glUniform3f(_phongShader->UEmissiveColor,
      _emissiveColor._r, _emissiveColor._g, _emissiveColor._b);
    glUniform3f(_phongShader->UGlobalAmbientColor,
      _globalAmbientColor._r, _globalAmbientColor._g, _globalAmbientColor._b);
    glUniform3f(_phongShader->UFogColor, 
      _fogColor._r, _fogColor._g, _fogColor._b);
    glUniform1f(_phongShader->UNearPlane, _fogNear);
    glUniform1f(_phongShader->UFarPlane, _fogFar);
    break;
  // GOURAUD SHADING
  case ShaderType::GOURAUD:
    _gouraudShader->Use();
    glUniformMatrix4fv(_gouraudShader->UProjection, 1, GL_TRUE, projection.array);
    glUniformMatrix4fv(_gouraudShader->UView, 1, GL_TRUE, view.array);
    glUniformMatrix4fv(_gouraudShader->UModel, 1, GL_TRUE, model.array);
    glUniform3f(_gouraudShader->UEmissiveColor,
      _emissiveColor._r, _emissiveColor._g, _emissiveColor._b);
    glUniform3f(_gouraudShader->UGlobalAmbientColor,
      _globalAmbientColor._r, _globalAmbientColor._g, _globalAmbientColor._b);
    glUniform3f(_gouraudShader->UFogColor,
      _fogColor._r, _fogColor._g, _fogColor._b);
    glUniform1f(_gouraudShader->UNearPlane, _fogNear);
    glUniform1f(_gouraudShader->UFarPlane, _fogFar);
    break;
  // BLINN SHADING
  case ShaderType::BLINN:
    _blinnShader->Use();
    glUniformMatrix4fv(_blinnShader->UProjection, 1, GL_TRUE, projection.array);
    glUniformMatrix4fv(_blinnShader->UView, 1, GL_TRUE, view.array);
    glUniformMatrix4fv(_blinnShader->UModel, 1, GL_TRUE, model.array);
    glUniform3f(_blinnShader->UEmissiveColor,
      _emissiveColor._r, _emissiveColor._g, _emissiveColor._b);
    glUniform3f(_blinnShader->UGlobalAmbientColor,
      _globalAmbientColor._r, _globalAmbientColor._g, _globalAmbientColor._b);
    glUniform3f(_blinnShader->UFogColor,
      _fogColor._r, _fogColor._g, _fogColor._b);
    glUniform1f(_blinnShader->UNearPlane, _fogNear);
    glUniform1f(_blinnShader->UFarPlane, _fogFar);
    break;
  // SOLID SHADING
  case ShaderType::SOLID:
    _solidShader->Use();
    glUniformMatrix4fv(_solidShader->UProjection, 1, GL_TRUE, projection.array);
    glUniformMatrix4fv(_solidShader->UView, 1, GL_TRUE, view.array);
    glUniformMatrix4fv(_solidShader->UModel, 1, GL_TRUE, model.array);
    break;
  default:
    break;
  }
  // drawing mesh
  glBindVertexArray(mesh_object->_vao);
  if (mesh_object->_showWireframe)
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  else
    glPolygonMode(GL_FRONT, GL_FILL);
  glDrawElements(GL_TRIANGLES, mesh_object->_elements, GL_UNSIGNED_INT, nullptr);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glBindVertexArray(0);

  // drawing normal lines
  if(mesh_object->_showVertexNormals || mesh_object->_showFaceNormals){
    _lineShader->Use();
    glUniformMatrix4fv(_lineShader->UProjection, 1, GL_TRUE, projection.array);
    glUniformMatrix4fv(_lineShader->UView, 1, GL_TRUE, view.array);
    glUniformMatrix4fv(_lineShader->UModel, 1, GL_TRUE, model.array);
  }
  if (mesh_object->_showVertexNormals) {
    // vertex normals
    glUniform3f(_lineShader->ULineColor,
      mesh_object->_vertexNormalColor._r, 
      mesh_object->_vertexNormalColor._g,
      mesh_object->_vertexNormalColor._b);
    glBindVertexArray(mesh_object->_vaoVertexNormal);
    glDrawArrays(GL_LINES, 0, mesh_object->_vertexNormalVertexCount);
    glBindVertexArray(0);
  }
  if (mesh_object->_showFaceNormals) {
    // face normals
    glUniform3f(_lineShader->ULineColor,
      mesh_object->_faceNormalColor._r,
      mesh_object->_faceNormalColor._g,
      mesh_object->_faceNormalColor._b);
    glBindVertexArray(mesh_object->_vaoFaceNormal);
    glDrawArrays(GL_LINES, 0, mesh_object->_faceNormalVertexCount);
    glBindVertexArray(0);
  }
}

void MeshRenderer::ReloadShader(ShaderType shader_type)
{
  // getting base shader type
  Shader * shader_to_reload;
  switch (shader_type)
  {
  case PHONG:
    shader_to_reload = _phongShader;
    break;
  case GOURAUD:
    shader_to_reload = _gouraudShader;
    break;
  case BLINN:
    shader_to_reload = _blinnShader;
    break;
  default:
    Error error("MeshRenderer.cpp", "ReloadShader");
    error.Add("ShaderType cannot be reloaded.");
    throw(error);
  }
  // disabling vertex attributes
  for (MeshObject * mesh_object : _meshObjects) {
    glBindVertexArray(mesh_object->_vao);
    shader_to_reload->DisableAttributes();
    glBindVertexArray(0);
  }
  // creating new shader
  switch (shader_type)
  {
  case PHONG:
    delete _phongShader;
    _phongShader = new PhongShader();
    shader_to_reload = _phongShader;
    break;
  case GOURAUD:
    delete _gouraudShader;
    _gouraudShader = new GouraudShader();
    shader_to_reload = _gouraudShader;
    break;
  case BLINN:
    delete _blinnShader;
    _blinnShader = new BlinnShader();
    shader_to_reload = _blinnShader;
    break;
  }
  // re-enabling vertex attributes
  for (MeshObject * mesh_object : _meshObjects) {
    glBindBuffer(GL_ARRAY_BUFFER, mesh_object->_vbo);
    glBindVertexArray(mesh_object->_vao);
    _phongShader->EnableAttributes();
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }
}

SolidShader * MeshRenderer::GetSolidShader()
{
  return _solidShader;
}

PhongShader * MeshRenderer::GetPhongShader()
{
  return _phongShader;
}

GouraudShader * MeshRenderer::GetGouraudShader()
{
  return _gouraudShader;
}

BlinnShader * MeshRenderer::GetBlinnShader()
{
  return _blinnShader;
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
  case ShaderType::SOLID:
    return 3;
  default:
    return 4;
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
    return ShaderType::SOLID;
  default:
    return ShaderType::NUMSHADERTYPES;
  }
}
