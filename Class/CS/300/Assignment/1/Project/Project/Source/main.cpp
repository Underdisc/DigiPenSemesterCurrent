

#include "External\Imgui\imgui.h"
#include "External\Imgui\imgui_impl_sdl_gl3.h"

#include "Graphics\SDLContext.h"
#include <GL\glew.h>
#include "Utility\OpenGLError.h"

#include "Graphics\OpenGLContext.h"
#include "Core\Time.h"
#include "Math\Matrix4.h"

#include "Graphics\Mesh.h"

#include <iostream>

void TestInit();
void TestUpdate();
void TestEnd();

inline void InitialUpdate()
{
  Time::Update();
  ImGui_ImplSdlGL3_NewFrame(SDLContext::SDLWindow());
  SDLContext::CheckEvents();
}

int main(int argc, char * argv[])
{
  ErrorLog::Clean();
  SDLContext::Create("CS 300 - Assignment 1", true, OpenGLContext::AdjustViewport);
  OpenGLContext::Initialize();

  ImGui_ImplSdlGL3_Init(SDLContext::SDLWindow());
  SDLContext::AddEventProcessor(ImGui_ImplSdlGL3_ProcessEvent);

  Mesh mesh("Resource/Model/horse.obj", Mesh::OBJ);


  TestInit();

  float r = 1.0f;
  bool diff = true;
  while (SDLContext::KeepOpen())
  {
    InitialUpdate();

    ImGui::Begin("Editor");
    if (ImGui::CollapsingHeader("Debug")) {
      ImGui::Text("FPS: %f", 1.0f / Time::DT());
    }
    ImGui::End();


    if (diff)
      r -= Time::DT();
    else
      r += Time::DT();
    if(r < 0.0f || r > 1.0f)
      diff = !diff;
    glClearColor(r, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    TestUpdate();
    ImGui::Render();
    OpenGLContext::Swap();
  }

  TestEnd();

  OpenGLContext::Purge();
  SDLContext::Purge();
}

#include "Graphics\Shader.h"
#define PI 3.141592653589f
Shader * draw_shader;

GLuint a_position;
GLuint u_projection;
GLuint u_model;
GLuint vbo_id;
GLuint ebo_id;
GLuint vao_id;

float cur_x = 0.0f;
float cur_y = 0.0f;
float cur_z = -2.0f;
float cur_scale = 1.0f;

inline void TestInit()
{
  draw_shader = new Shader("Resource/Shader/default.vert", "Resource/Shader/default.frag");
  a_position = draw_shader->GetAttribLocation("APosition");
  u_model = draw_shader->GetUniformLocation("UModel");
  u_projection = draw_shader->GetUniformLocation("UProjection");

  GLfloat vertices[] = {
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,

    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
  };
  GLuint indicies[] = {
    0, 2, 1,
    0, 3, 2,
    1, 2, 6,
    1, 6, 5,
    5, 6, 7,
    5, 7, 4,
    4, 3, 0,
    4, 7, 3,
    0, 1, 5,
    0, 5, 4,
    3, 7, 6,
    3, 6, 2
  };

  glGenVertexArrays(1, &vao_id);
  glGenBuffers(1, &vbo_id);
  glGenBuffers(1, &ebo_id);
  glBindVertexArray(vao_id);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_id);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW);

  glVertexAttribPointer(a_position, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
  glEnableVertexAttribArray(a_position);
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  try
  {
    GLenum gl_error = glGetError();
    OPENGLERRORCHECK("main.cpp", "TestInit()", "During initialization", gl_error)
  }
  catch (const Error & error)
  {
    ErrorLog::Write(error);
  }

}

#include "Core\Input.h"

inline void TestUpdate()
{

  if (Input::KeyDown(W))
    cur_z += Time::DT();
  if (Input::KeyDown(S))
    cur_z -= Time::DT();
  if (Input::KeyDown(D))
    cur_x += Time::DT();
  if (Input::KeyDown(A))
    cur_x -= Time::DT();
  if (Input::KeyDown(E))
    cur_y += Time::DT();
  if (Input::KeyDown(Q))
    cur_y -= Time::DT();
  if (Input::KeyDown(R))
    cur_scale += Time::DT();
  if (Input::KeyDown(F))
    cur_scale -= Time::DT();

  Math::Matrix4 translation;
  translation.Translate(cur_x, cur_y, cur_z);
  Math::Matrix4 scale;
  scale.Scale(cur_scale, cur_scale, cur_scale);
  Math::Matrix4 model = translation * scale;

  Math::Matrix4 projection = Math::Matrix4::Perspective(PI/2.0f, OpenGLContext::AspectRatio(), 1.0f, 100.0f);
  draw_shader->Use();
  glUniformMatrix4fv(u_projection, 1, GL_TRUE, projection.array);
  glUniformMatrix4fv(u_model, 1, GL_TRUE, model.array);

  glBindVertexArray(vao_id);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


  try
  {
    GLenum gl_error = glGetError();
    OPENGLERRORCHECK("main.cpp", "TestuUpdate()", "During Update", gl_error)
  }
  catch (const Error & error)
  {
    ErrorLog::Write(error);
  }
}

inline void TestEnd()
{
  draw_shader->Purge();
  delete draw_shader;
}