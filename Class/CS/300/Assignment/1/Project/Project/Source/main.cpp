

#include "External\Imgui\imgui.h"
#include "External\Imgui\imgui_impl_sdl_gl3.h"

#include "Graphics\SDLContext.h"
#include <GL\glew.h>
#include "Utility\OpenGLError.h"

#include "Graphics\OpenGLContext.h"
#include "Core\Time.h"
#include "Math\Matrix4.h"
#include "Math\Vector3.h"

#include "Graphics\Mesh.h"

#include <iostream>

void EditorUpdate();

void TestInit();
void TestUpdate();
void TestEnd();

inline void InitialUpdate()
{
  Time::Update();
  ImGui_ImplSdlGL3_NewFrame(SDLContext::SDLWindow());
  SDLContext::CheckEvents();
}

float clear_color[] { 0.4f, 0.4f, 0.4f };

int main(int argc, char * argv[])
{
  ErrorLog::Clean();
  SDLContext::Create("CS 300 - Assignment 1", true, OpenGLContext::AdjustViewport);
  OpenGLContext::Initialize();

  ImGui_ImplSdlGL3_Init(SDLContext::SDLWindow());
  SDLContext::AddEventProcessor(ImGui_ImplSdlGL3_ProcessEvent);

  TestInit();

  float r = 1.0f;
  bool diff = true;
  while (SDLContext::KeepOpen())
  {
    InitialUpdate();
    EditorUpdate();
    glClearColor(clear_color[0], clear_color[1], clear_color[2], 1.0f);
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
GLuint a_normal;
GLuint u_projection;
GLuint u_model;
GLuint vbo_id;
GLuint ebo_id;
GLuint vao_id;

Math::Vector3 cur_tran(0.0f, 0.0f, -2.0f);
float cur_scale = 1.0f;
Math::Matrix4 rotation;

unsigned num_elements;

inline void EditorUpdate()
{
  ImGui::ShowTestWindow();
  ImGui::Begin("Editor");
  if (ImGui::CollapsingHeader("Debug")) {
    ImGui::Text("FPS: %f", 1.0f / Time::DT());
    ImGui::Separator();
  }
  if (ImGui::CollapsingHeader("Global")) {
    ImGui::ColorEdit3("Clear Color", clear_color);
    ImGui::Separator();
  }
  if (ImGui::CollapsingHeader("Mesh")) {
    
  }
  ImGui::End();
}

inline void TestInit()
{

  Math::Matrix4 test;
  test.Rotate(1,0,0, PI);

  draw_shader = new Shader("Resource/Shader/default.vert", "Resource/Shader/default.frag");
  a_position = draw_shader->GetAttribLocation("APosition");
  a_normal = draw_shader->GetAttribLocation("ANormal");
  u_model = draw_shader->GetUniformLocation("UModel");
  u_projection = draw_shader->GetUniformLocation("UProjection");

  Mesh mesh("Resource/Model/horse.obj", Mesh::OBJ);
  num_elements = mesh.IndexDataSize();

  GLfloat vertices[]{
    0.5f, 0.5f, 0.5f,        0.5f, 0.5f, 0.5f,
    0.5f, 0.5f, -0.5f,       0.5f, 0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,      0.5f, -0.5f, -0.5f,
    0.5f, -0.5f, 0.5f,       0.5f, -0.5f, 0.5f,
    -0.5f, 0.5f, 0.5f,       -0.5f, 0.5f, 0.5f,
    -0.5f, 0.5f, -0.5f,      -0.5f, 0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,     -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, 0.5f,      -0.5f, -0.5f, 0.5f
  };

  GLuint indicies[]{
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
    3, 6, 4
  };

  glGenVertexArrays(1, &vao_id);
  glGenBuffers(1, &vbo_id);
  glGenBuffers(1, &ebo_id);
  glBindVertexArray(vao_id);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
  glBufferData(GL_ARRAY_BUFFER, mesh.VertexDataSizeBytes(), mesh.VertexData(), GL_STATIC_DRAW);
  //glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_id);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.IndexDataSizeBytes(), mesh.IndexData(), GL_STATIC_DRAW);
  //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW);
  glVertexAttribPointer(a_position, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr);
  glEnableVertexAttribArray(a_position);
  glVertexAttribPointer(a_normal, 3, GL_FLOAT, GL_TRUE, 6 * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));
  glEnableVertexAttribArray(a_normal);
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
  rotation.SetIdentity();
}

#include "Core\Input.h"

inline void ManageInput()
{
  if (Input::KeyDown(W))
    cur_tran.z += Time::DT();
  if (Input::KeyDown(S))
    cur_tran.z -= Time::DT();
  if (Input::KeyDown(D))
    cur_tran.x += Time::DT();
  if (Input::KeyDown(A))
    cur_tran.x -= Time::DT();
  if (Input::KeyDown(E))
    cur_tran.y += Time::DT();
  if (Input::KeyDown(Q))
    cur_tran.y -= Time::DT();
  if (Input::KeyDown(R))
    cur_scale += Time::DT() * 10.0f;
  if (Input::KeyDown(F))
    cur_scale -= Time::DT() * 10.0f;
  if (Input::MouseButtonDown(LEFT)) {
    std::pair<int, int> mouse_motion = Input::MouseMotion();
    if (mouse_motion.first != 0 && mouse_motion.second != 0) {
      Math::Vector3 normal_mouse_motion((float)mouse_motion.first, (float)mouse_motion.second, 0.0f);
      float mag = normal_mouse_motion.Length();
      normal_mouse_motion.Normalize();
      Math::Vector3 normal_inverse_view(0.0f, 0.0f, 1.0f);
      normal_inverse_view.Normalize();
      Math::Vector3 normal_rot_axis = Math::Cross(normal_mouse_motion, normal_inverse_view);
      Math::Matrix4 new_rotation;
      new_rotation.Rotate(normal_rot_axis, mag / 100.0f);
      rotation = new_rotation * rotation;
    }
  }
}

inline void TestUpdate()
{
  ImGuiIO & imgui_io = ImGui::GetIO();
  if(!imgui_io.WantCaptureMouse)
    ManageInput();

  Math::Matrix4 translation;
  translation.Translate(cur_tran.x, cur_tran.y, cur_tran.z);
  Math::Matrix4 scale;
  scale.Scale(cur_scale, cur_scale, cur_scale);
  Math::Matrix4 model = translation * rotation * scale;

  Math::Matrix4 projection = Math::Matrix4::Perspective(PI/2.0f, OpenGLContext::AspectRatio(), 0.1f, 100.0f);
  draw_shader->Use();
  glUniformMatrix4fv(u_projection, 1, GL_TRUE, projection.array);
  glUniformMatrix4fv(u_model, 1, GL_TRUE, model.array);

  glBindVertexArray(vao_id);
  glPolygonMode(GL_FRONT, GL_FILL);
  glDrawElements(GL_TRIANGLES, num_elements, GL_UNSIGNED_INT, nullptr);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


  try
  {
    GLenum gl_error = glGetError();
    OPENGLERRORCHECK("main.cpp", "TestUpdate()", "During Update", gl_error)
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