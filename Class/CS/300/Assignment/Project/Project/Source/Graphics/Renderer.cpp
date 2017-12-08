/* All content(c) 2017 - 2018 DigiPen(USA) Corporation, all rights reserved. */
#include "Renderer.h"

#include "../Editor/Editor.h"
#include "../Math/MathFunctions.h"
#include "../Utility/OpenGLError.h"

#define PI 3.141592653589f
#define PI2 6.28318530718f

// static initializations
Mesh * Renderer::_mesh = nullptr;
MeshRenderer::MeshObject * Renderer::_meshObject = nullptr;
MeshRenderer::MeshObject * Renderer::_sphereMeshObject = nullptr;

TextureObject * Renderer::_diffuseTextureObject = nullptr;
TextureObject * Renderer::_specularTextureObject = nullptr;
TextureObject * Renderer::_normalTextureObject = nullptr;

Skybox * Renderer::_skybox = nullptr;

#include <iostream>

void Renderer::Initialize(Mesh & mesh) 
{
  // mesh stuff
  _meshObject = MeshRenderer::Upload(&mesh);
  Mesh sphere_mesh(MODEL_PATH + std::string("sphere.obj"), Mesh::OBJ);
  _sphereMeshObject = MeshRenderer::Upload(&sphere_mesh);

  // texture stuff
  _diffuseTextureObject = TexturePool::Upload("Resource/Texture/diffuse.tga");
  _specularTextureObject = TexturePool::Upload("Resource/Texture/specular.tga");
  _normalTextureObject = TexturePool::Upload("Resource/Texture/normal.png");
  // IT'S OK I KNOW YOU'RE WONDERING WHY YOUR NORMAL MAPS AREN'T SHOWING UP
  // JUST PUT THESE BEFORE YOUR MESH RENDER
  TexturePool::Bind(_diffuseTextureObject, 0);
  TexturePool::Bind(_specularTextureObject, 1);
  TexturePool::Bind(_normalTextureObject, 2);

  // skybox
  _skybox = new Skybox("Resource/Texture/Skybox/Test/",
    "up.tga", "dn.tga", "lf.tga", "rt.tga", "ft.tga", "bk.tga");
  _skybox->Upload();
}

void Renderer::Purge()
{
  MeshRenderer::Unload(_sphereMeshObject);
  TexturePool::Unload(_diffuseTextureObject);
  TexturePool::Unload(_specularTextureObject);
  TexturePool::Unload(_normalTextureObject);
  _skybox->Unload();
  delete _skybox;
}

void Renderer::Render(const Math::Matrix4 & projection, 
  const Math::Matrix4 & view, const Math::Vector3 & view_position, bool mesh)
{
  _skybox->Render(projection, view);

  SolidShader * solid_shader = MeshRenderer::GetSolidShader();
  PhongShader * phong_shader = MeshRenderer::GetPhongShader();
  GouraudShader * gouraud_shader = MeshRenderer::GetGouraudShader();
  BlinnShader * blinn_shader = MeshRenderer::GetBlinnShader();
  Math::Matrix4 model;
  Math::Matrix4 translate;
  Math::Matrix4 rotate;
  Math::Matrix4 scale;

  // render lights
  solid_shader->Use();

  for (int i = 0; i < Light::_activeLights; ++i) {
    translate.Translate(Editor::lights[i]._position.x, Editor::lights[i]._position.y, Editor::lights[i]._position.z);
    scale.Scale(0.25f, 0.25f, 0.25f);
    model = translate * scale;
    Color & color = Editor::lights[i]._diffuseColor;
    glUniform3f(solid_shader->UColor, color._r, color._g, color._b);
    MeshRenderer::Render(_sphereMeshObject, MeshRenderer::SOLID, projection, view, model);
  }

  translate.Translate(Editor::trans.x, Editor::trans.y, Editor::trans.z);
  scale.Scale(Editor::cur_scale, Editor::cur_scale, Editor::cur_scale);
  Math::ToMatrix4(Editor::rotation, &rotate);
  model = translate * rotate * scale;

  switch (Editor::shader_in_use)
  {
    //PHONG SHADER
  case MeshRenderer::ShaderType::PHONG:
    phong_shader->Use();
    glUniform3f(phong_shader->UCameraPosition, view_position.x, view_position.y, view_position.z);
    glUniform1i(phong_shader->UActiveLights, Light::_activeLights);
    for (int i = 0; i < Light::_activeLights; ++i)
      Editor::lights[i].SetUniforms(i, phong_shader);
    break;
    // GOURAUD SHADER
  case MeshRenderer::ShaderType::GOURAUD:
    gouraud_shader->Use();
    glUniform3f(gouraud_shader->UCameraPosition, view_position.x, view_position.y, view_position.z);
    glUniform1i(gouraud_shader->UActiveLights, Light::_activeLights);
    glUniform1i(gouraud_shader->UActiveLights, Light::_activeLights);
    for (int i = 0; i < Light::_activeLights; ++i)
      Editor::lights[i].SetUniforms(i, gouraud_shader);
    break;
  case MeshRenderer::ShaderType::BLINN:
    blinn_shader->Use();
    glUniform3f(blinn_shader->UCameraPosition, view_position.x, view_position.y, view_position.z);
    glUniform1i(blinn_shader->UActiveLights, Light::_activeLights);
    for (int i = 0; i < Light::_activeLights; ++i)
      Editor::lights[i].SetUniforms(i, blinn_shader);
    break;
  default:
    break;
  }
  // rendering mesh
  if(mesh)
    MeshRenderer::Render(_meshObject, Editor::shader_in_use, projection, view, model);
  // disable writing to error strings
  try
  {
    GLenum gl_error = glGetError();
    OPENGLERRORCHECK("Renderer.cpp", "Render()", "During Render", gl_error)
  }
  catch (const Error & error)
  {
    ErrorLog::Write(error);
  }
}

void Renderer::ReplaceMesh(Mesh & mesh)
{
  MeshRenderer::Unload(_meshObject);
  _meshObject = MeshRenderer::Upload(&mesh);
}