/* All content(c) 2017 DigiPen(USA) Corporation, all rights reserved. */
/*****************************************************************************/
/*!
\file main.cpp
\author Connor Deakin
\par E-mail: connor.deakin\@digipen.edu
\par Project: CS 300
\date 02/10/2017
\brief
  Entry point for the program. Implementation handles all editor 
  functionality, other interface code, and GL code from drawing.
*/
/*****************************************************************************/


#include "Graphics\Light.h"
#include "Graphics\Material.h"
#include "Graphics\Renderer.h"
#include "Graphics\Texture\TexturePool.h"

#include <GL\glew.h>
#include "Utility\OpenGLError.h"

#include "Graphics\OpenGLContext.h"
#include "Core\Time.h"
#include "Math\Matrix4.h"
#include "Math\Vector3.h"
#include "Math\EulerAngles.h"
#include "Math\MathFunctions.h"

#include "Graphics\Mesh\Mesh.h"
#include "Graphics\Mesh\MeshRenderer.h"
#include "Graphics\Shader\ShaderLibrary.h"
#include "Graphics\Camera.h"

#include "Core\Input.h"
#include "Core\Framer.h"
#include "Editor\Editor.h"

#define PI 3.141592653589f
#define PI2 6.28318530718f
#define FPS 60
#define MODEL_PATH "Resource/Model/"
#define FILENAME_BUFFERSIZE 50

// GLOBAL


Mesh * mesh;
MeshRenderer::MeshObject * mesh_object;
MeshRenderer::MeshObject * sphere_mesh_object;
MeshRenderer::MeshObject * plane_mesh_object;

TextureObject * diffuse_texture_object;
TextureObject * specular_texture_object;


Camera camera(Math::Vector3(0.0f, 1.0f, 0.0f));
float movespeed = 1.0f;
float mouse_sensitivity = 0.3f;
float mouse_wheel_sensitivity = 2.0f;
// Global

void LoadMesh(const std::string & model);
void Update();
void Render();

inline void ManageInput()
{
  if (Input::MouseButtonDown(LEFT)) {
    std::pair<int, int> mouse_motion = Input::MouseMotion();

    if (mouse_motion.first != 0)
      Editor::rotation.Angles.y += mouse_motion.first / 100.0f;
    if (mouse_motion.second != 0) {
      Editor::rotation.Angles.x -= mouse_motion.second / 100.0f;
    }
  }

  movespeed += movespeed * mouse_wheel_sensitivity * Time::DT() * Input::MouseWheelMotion();
  if (Input::KeyDown(W))
    camera.MoveBack(-Time::DT() * movespeed);
  if (Input::KeyDown(S))
    camera.MoveBack(Time::DT() * movespeed);
  if (Input::KeyDown(A))
    camera.MoveRight(-Time::DT() * movespeed);
  if (Input::KeyDown(D))
    camera.MoveRight(Time::DT() * movespeed);
  if (Input::KeyDown(Q))
    camera.MoveGlobalUp(-Time::DT() * movespeed);
  if (Input::KeyDown(E))
    camera.MoveGlobalUp(Time::DT() * movespeed);
  if (Input::MouseButtonDown(RIGHT) && !Editor::rotate_camera) {
    std::pair<int, int> mouse_motion = Input::MouseMotion();
    camera.MoveYaw((float)mouse_motion.first * Time::DT() * mouse_sensitivity);
    camera.MovePitch(-(float)mouse_motion.second * Time::DT() * mouse_sensitivity);
  }
}

void LoadOtherMeshes()
{
  Mesh sphere_mesh(MODEL_PATH + std::string("sphere.obj"), Mesh::OBJ);
  sphere_mesh_object = MeshRenderer::Upload(&sphere_mesh);
  Mesh plane_mesh(MODEL_PATH + std::string("plane.obj"), Mesh::OBJ);
  plane_mesh_object = MeshRenderer::Upload(&plane_mesh);
}

inline void InitialUpdate()
{
  Time::Update();
  SDLContext::CheckEvents();
}

inline void Clear()
{
  Color & fog_color = MeshRenderer::_fogColor;
  glClearColor(fog_color._r, fog_color._g, fog_color._b, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

int main(int argc, char * argv[])
{
  ErrorLog::Clean();
  SDLContext::Create("CS 300 - Assignment 2", true, OpenGLContext::AdjustViewport);
  OpenGLContext::Initialize();
  MeshRenderer::Initialize();
  Editor::Initialize();


  LoadMesh(Editor::current_mesh);
  LoadOtherMeshes();
  camera.MoveBack(2.0f);
  // starting main program
  glEnable(GL_DEPTH_TEST);
  Framer::Lock(FPS);
  while (SDLContext::KeepOpen())
  {
    // frame start
    Framer::Start();
    InitialUpdate();
    Editor::Update(mesh, mesh_object, LoadMesh);
    Update();
    Clear();
    Render();
    Editor::Render();
    OpenGLContext::Swap();
    // frame end
    Framer::End();
  }

  Mesh::Purge(mesh);
  MeshRenderer::Purge();
  OpenGLContext::Purge();
  SDLContext::Purge();
}

void LoadMesh(const std::string & model)
{
  // loading new mesh
  Mesh * new_mesh;
  std::string mesh_path = MODEL_PATH + model;
  try {
    new_mesh = Mesh::Load(mesh_path, Mesh::OBJ);
  }
  catch (const Error & error) {
    return;
  }
  // deleting old mesh
  if (mesh){
    MeshRenderer::Unload(mesh_object);
    Mesh::Purge(mesh);
  }
  // uploading mesh data to gpu
  new_mesh->SetNormalLineLengthMeshRelative(0.1f);
  mesh_object = MeshRenderer::Upload(new_mesh);
  // new mesh loaded
  mesh = new_mesh;
  Editor::current_mesh = model;
  try
  {
    GLenum gl_error = glGetError();
    OPENGLERRORCHECK("main.cpp", "Initialize()", "During initialization", gl_error)
  }
  catch (const Error & error)
  {
    ErrorLog::Write(error);
  }
}

inline void RotateCamera()
{
  float camera_angle = Time::TotalTimeScaled() * Editor::camera_rotate_speed;
  Math::Vector3 new_position;
  new_position.x = Math::Cos(camera_angle) * Editor::camera_distance;
  new_position.y = Math::Sin(camera_angle);
  new_position.z = Math::Sin(camera_angle) * Editor::camera_distance;
  camera.SetPosition(new_position);
  camera.LookAt(Math::Vector3(Editor::trans.x, Editor::trans.y, Editor::trans.z));
}

inline void RotateLights()
{
  for (int i = 0; i < Light::_activeLights; ++i) {
    float percentage = (float)i / (float)Light::_activeLights;
    float light_angle = percentage * PI2 + Time::TotalTimeScaled() * Editor::rotate_light_speed;
    Light & light = Editor::lights[i];

    float cos_angle = Math::Cos(light_angle);
    float sin_angle = Math::Sin(light_angle);
    light._position.x = cos_angle * Editor::cur_scale + 0.5f * cos_angle + Editor::trans.x;
    light._position.y = Editor::trans.y;
    light._position.z = sin_angle * Editor::cur_scale + 0.5f * sin_angle + Editor::trans.z;

    light._direction.x = -cos_angle;
    light._direction.y = 0.0f;
    light._direction.z = -sin_angle;
  }


}

inline void Update()
{
  if(!Editor::MouseHovering())
    ManageInput();
  if(Editor::rotating_lights)
    RotateLights();
  if(Editor::rotate_camera)
    RotateCamera();
}

inline void Render()
{
  SolidShader * solid_shader = MeshRenderer::GetSolidShader();
  PhongShader * phong_shader = MeshRenderer::GetPhongShader();
  GouraudShader * gouraud_shader = MeshRenderer::GetGouraudShader();
  BlinnShader * blinn_shader = MeshRenderer::GetBlinnShader();
  Math::Matrix4 projection;
  Math::Matrix4 model;
  Math::Matrix4 translate;
  Math::Matrix4 rotate;
  Math::Matrix4 scale;
  projection = Math::Matrix4::Perspective(PI / 2.0f,
    OpenGLContext::AspectRatio(), MeshRenderer::_nearPlane,
    MeshRenderer::_farPlane);

  // render lights
  solid_shader->Use();

  for (int i = 0; i < Light::_activeLights; ++i) {
    translate.Translate(Editor::lights[i]._position.x, Editor::lights[i]._position.y, Editor::lights[i]._position.z);
    scale.Scale(0.25f, 0.25f, 0.25f);
    model = translate * scale;
    Color & color = Editor::lights[i]._diffuseColor;
    glUniform3f(solid_shader->UColor, color._r, color._g, color._b);
    MeshRenderer::Render(sphere_mesh_object, MeshRenderer::SOLID, projection, camera.ViewMatrix(), model);
  }

  translate.Translate(Editor::trans.x, Editor::trans.y, Editor::trans.z);
  scale.Scale(Editor::cur_scale, Editor::cur_scale, Editor::cur_scale);
  Math::ToMatrix4(Editor::rotation, &rotate);
  model = translate * rotate * scale;

  const Math::Vector3 & cpos = camera.GetPosition();
  switch (Editor::shader_in_use)
  {
  //PHONG SHADER
  case MeshRenderer::ShaderType::PHONG:
    phong_shader->Use();
    glUniform3f(phong_shader->UCameraPosition, cpos.x, cpos.y, cpos.z);
    glUniform1i(phong_shader->UActiveLights, Light::_activeLights);
    for (int i = 0; i < Light::_activeLights; ++i)
      Editor::lights[i].SetUniforms(i, phong_shader);
    Editor::material.SetUniforms(phong_shader);
    break;
  // GOURAUD SHADER
  case MeshRenderer::ShaderType::GOURAUD:
    gouraud_shader->Use();
    glUniform3f(gouraud_shader->UCameraPosition, cpos.x, cpos.y, cpos.z);
    glUniform1i(gouraud_shader->UActiveLights, Light::_activeLights);
    glUniform1i(gouraud_shader->UActiveLights, Light::_activeLights);
    for (int i = 0; i < Light::_activeLights; ++i)
      Editor::lights[i].SetUniforms(i, gouraud_shader);
    Editor::material.SetUniforms(gouraud_shader);
    break;
  case MeshRenderer::ShaderType::BLINN:
    blinn_shader->Use();
    glUniform3f(blinn_shader->UCameraPosition, cpos.x, cpos.y, cpos.z);
    glUniform1i(blinn_shader->UActiveLights, Light::_activeLights);
    for (int i = 0; i < Light::_activeLights; ++i)
      Editor::lights[i].SetUniforms(i, blinn_shader);
    Editor::material.SetUniforms(blinn_shader);
    break;
  default:
    break;
  }
  // rendering mesh
  MeshRenderer::Render(mesh_object, Editor::shader_in_use, projection, camera.ViewMatrix(), model);
  translate.Translate(0.0, -5.0f, 0.0f);
  scale.Scale(20.0f, 20.0f, 20.0f);
  model = translate * scale;
  MeshRenderer::Render(plane_mesh_object, Editor::shader_in_use, projection, camera.ViewMatrix(), model);
  // disable writing to error strings
  /*try
  {
    GLenum gl_error = glGetError();
    OPENGLERRORCHECK("main.cpp", "Update()", "During Update", gl_error)
  }
  catch (const Error & error)
  {
    ErrorLog::Write(error);
  }*/
}