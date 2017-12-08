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

#include <iostream>

#include "Graphics\Light.h"
#include "Graphics\Material.h"
#include "Graphics\Renderer.h"
#include "Graphics\Skybox.h"
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
#include "Graphics\Shader\ShaderManager.h"
#include "Graphics\Camera.h"

#include "Core\Input.h"
#include "Core\Framer.h"
#include "Editor\Editor.h"


// make sure to do this
//#include "vld.h"

#define PI 3.141592653589f
#define PI2 6.28318530718f
#define FPS 60
#define MODEL_PATH "Resource/Model/"
#define FILENAME_BUFFERSIZE 50

Mesh * mesh;

// GLOBAL
Camera camera(Math::Vector3(0.0f, 1.0f, 0.0f));
float movespeed = 1.0f;
float mouse_sensitivity = 0.3f;
float mouse_wheel_sensitivity = 2.0f;
// Global

void LoadMesh(const std::string & model);

void Update();

void TempInit();

void Draw();

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

//--------------------// Main //--------------------//

int main(int argc, char * argv[])
{
  ErrorLog::Clean();
  SDLContext::Create("CS 300 - Assignment 4", true, OpenGLContext::AdjustViewport);
  OpenGLContext::Initialize();
  ShaderManager::Initialize();
  MeshRenderer::Initialize();
  Editor::Initialize();


  LoadMesh(Editor::current_mesh);
  Renderer::Initialize(*mesh);

  camera.MoveBack(2.0f);

  // loading normal map from specular bump map
  Texture bump_map("Resource/Texture/specular.tga");
  bump_map.CreateNormalMap("Resource/Texture/normal_map.png", 2.0f / 255.0f);


  // need to get rid of this
  TempInit();

  // starting main program
  glEnable(GL_DEPTH_TEST);
  Framer::Lock(FPS);
  while (SDLContext::KeepOpen())
  {
    // frame start
    Framer::Start();
    InitialUpdate();
    Editor::Update(mesh, Renderer::_meshObject, LoadMesh);
    Update();
    Draw();
    Editor::Render();
    OpenGLContext::Swap();
    // frame end
    Framer::End();
  }

  Mesh::Purge(mesh);
  Renderer::Purge();
  MeshRenderer::Purge();
  ShaderManager::Purge();
  OpenGLContext::Purge();
  SDLContext::Purge();
}

//--------------------// Other //--------------------//

void LoadMesh(const std::string & model)
{
  // loading new mesh
  Mesh * new_mesh;
  std::string mesh_path = MODEL_PATH + model;
  try {
    new_mesh = Mesh::Load(mesh_path, Mesh::OBJ, MESH_MAPPING_SPHERICAL);
  }
  catch (const Error & error) {
    return;
  }
  // deleting old mesh
  if (mesh){
    MeshRenderer::Unload(Renderer::_meshObject);
    Mesh::Purge(mesh);
  }
  // uploading mesh data to gpu
  new_mesh->SetNormalLineLengthMeshRelative(0.1f);
  Renderer::_meshObject = MeshRenderer::Upload(new_mesh);
  // new mesh loaded
  mesh = new_mesh;
  Editor::current_mesh = model;
  Renderer::ReplaceMesh(*mesh);
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

//-----// Framebuffer //-----//

class Framebuffer
{
public:
  Framebuffer()
  {}

  void Initialize(unsigned int width, unsigned int height)
  {
    // create framebuffer
    glGenFramebuffers(1, &_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
    // create framebuffer texture
    glGenTextures(1, &_tbo);
    glBindTexture(GL_TEXTURE_2D, _tbo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _tbo, 0);
    // create renderbuffer
    glGenRenderbuffers(1, &_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, _rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _rbo);
    // unbind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    try{
      GLenum gl_error = glGetError();
      OPENGLERRORCHECK("main.cpp", "Framebuffer::Initialize", "During Framebuffer creation", gl_error);
    }
    catch(const Error & error){
      ErrorLog::Write(error);
    }
  }

  void Bind()
  {
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
  }

  static void BindDefault() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }
private:
  GLuint _fbo;
  GLuint _tbo;
  GLuint _rbo;
};

Framebuffer fb_up;
Framebuffer fb_down;
Framebuffer fb_left;
Framebuffer fb_right;
Framebuffer fb_front;
Framebuffer fb_back;

void TempInit()
{
  fb_up.Initialize(512, 512);
  fb_down.Initialize(512, 512);
  fb_left.Initialize(512, 512);
  fb_right.Initialize(512, 512);
  fb_front.Initialize(512, 512);
  fb_back.Initialize(512, 512);
}

//--------------------// Draw //--------------------//

void Draw()
{
  Clear();
  Math::Matrix4 environment_projection = Math::Matrix4::Perspective(PI / 2.0f,
    1.0f, MeshRenderer::_nearPlane, MeshRenderer::_farPlane);


  Math::Matrix4 translation_term(
    1.0f, 0.0f, 0.0f, -Editor::trans.x,
    0.0f, 1.0f, 0.0f, -Editor::trans.y,
    0.0f, 0.0f, 1.0f, -Editor::trans.z,
    0.0f, 0.0f, 0.0f, 1.0f);

  Math::Vector4 pos_x(1.0f, 0.0f, 0.0f, 0.0f);
  Math::Vector4 pos_y(0.0f, 1.0f, 0.0f, 0.0f);
  Math::Vector4 pos_z(0.0f, 0.0f, 1.0f, 0.0f);
  Math::Vector4 neg_x(-1.0f, 0.0f, 0.0f, 0.0f);
  Math::Vector4 neg_y(0.0f, -1.0f, 0.0f, 0.0f);
  Math::Vector4 neg_z(0.0f, 0.0f, -1.0f, 0.0f);
  Math::Vector4 basis_w(0.0f, 0.0f, 0.0f, 1.0f);

  // linear parts
  Math::Matrix4 up(pos_x, pos_z, neg_y, basis_w);
  up.Transpose();
  Math::Matrix4 down(pos_x, neg_z, pos_y, basis_w);
  down.Transpose();
  Math::Matrix4 left(neg_z, pos_y, pos_x, basis_w);
  left.Transpose();
  Math::Matrix4 right(pos_z, pos_y, neg_x, basis_w);
  right.Transpose();
  Math::Matrix4 front(pos_x, pos_y, pos_z, basis_w);
  front.Transpose();
  Math::Matrix4 back(neg_x, pos_y, neg_z, basis_w);
  back.Transpose();

  up = up * translation_term;
  down = down * translation_term;
  left = left * translation_term;
  right = right * translation_term;
  front = front * translation_term;
  back = back * translation_term;


  // up render
  fb_up.Bind();
  Clear();
  Renderer::Render(environment_projection, up, Editor::trans, true);
  Framebuffer::BindDefault();
  // down render
  fb_down.Bind();
  Clear();
  Renderer::Render(environment_projection, down, Editor::trans, true);
  Framebuffer::BindDefault();
  // left render
  fb_left.Bind();
  Clear();
  Renderer::Render(environment_projection, left, Editor::trans, true);
  Framebuffer::BindDefault();
  // right render
  fb_right.Bind();
  Clear();
  Renderer::Render(environment_projection, right, Editor::trans, true);
  Framebuffer::BindDefault();
  // front render
  fb_front.Bind();
  Clear();
  Renderer::Render(environment_projection, front, Editor::trans, true);
  Framebuffer::BindDefault();
  // back render
  fb_back.Bind();
  Clear();
  Renderer::Render(environment_projection, back, Editor::trans, true);
  Framebuffer::BindDefault();
  
  
  Math::Matrix4 final_projection = Math::Matrix4::Perspective(PI / 2.0f,
    OpenGLContext::AspectRatio(), MeshRenderer::_nearPlane,
    MeshRenderer::_farPlane);
  Renderer::Render(final_projection, camera.ViewMatrix(), camera.GetPosition(), true);
}
