

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
#include "Graphics\ShaderLibrary.h"

#include <iostream>


#define PI 3.141592653589f
// GLOBAL
PhongShader * phong_shader_p;
LineShader * line_shader_p;
Mesh * mesh_p;

GLuint mesh_vbo;
GLuint mesh_ebo;
GLuint mesh_vao;

GLuint vert_norm_line_vbo;
GLuint vert_norm_line_vao;

GLuint face_norm_line_vbo;

Math::Vector3 cur_tran(0.0f, 0.0f, -2.0f);
float cur_scale = 1.0f;
float scale_speed = 2.0f;
Math::Matrix4 rotation;
bool line_mode = false;
unsigned num_elements;

bool show_facen = false;
bool show_vertn = false;
// GLOBAL


void Initialize();
void Update();
void EditorUpdate();
void Render();
void End();

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

  Initialize();

  float r = 1.0f;
  bool diff = true;
  glEnable(GL_DEPTH_TEST);
  while (SDLContext::KeepOpen())
  {
    InitialUpdate();
    Update();
    EditorUpdate();
    glClearColor(clear_color[0], clear_color[1], clear_color[2], 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    Render();
    ImGui::Render();
    OpenGLContext::Swap();
  }

  End();

  OpenGLContext::Purge();
  SDLContext::Purge();
}

inline void EditorUpdate()
{
  ImGui::ShowTestWindow();
  ImGui::Begin("Editor");
  if (ImGui::CollapsingHeader("Debug")) {
    ImGui::Text("FPS: %f", 1.0f / Time::DT());
  }
  ImGui::Separator();
  if (ImGui::CollapsingHeader("Global")) {
    ImGui::ColorEdit3("Clear Color", clear_color);
    ImGui::DragFloat("Time Scale", &Time::m_TimeScale, 0.01f);
  }
  ImGui::Separator();
  if (ImGui::CollapsingHeader("Mesh")) {
    ImGui::Text("Vertex Count: %d", mesh_p->VertexCount());
    ImGui::Text("Face Count: %d", mesh_p->FaceCount());
    ImGui::Separator();
    ImGui::Checkbox("Show Face Normals", &show_facen);
    ImGui::Checkbox("Show Vertex Normals", &show_vertn);
  }
  ImGui::Separator();
  ImGui::End();
}

inline void Initialize()
{
  phong_shader_p = new PhongShader();
  line_shader_p = new LineShader();

  mesh_p = new Mesh("Resource/Model/horse.obj", Mesh::OBJ);
  Mesh & mesh = *mesh_p;
  num_elements = mesh.IndexDataSize();
  // uploading mesh data
  glGenVertexArrays(1, &mesh_vao);
  glGenBuffers(1, &mesh_vbo);
  glGenBuffers(1, &mesh_ebo);
  glBindVertexArray(mesh_vao);
  glBindBuffer(GL_ARRAY_BUFFER, mesh_vbo);
  glBufferData(GL_ARRAY_BUFFER, mesh.VertexDataSizeBytes(), mesh.VertexData(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.IndexDataSizeBytes(), mesh.IndexData(), GL_STATIC_DRAW);
  glVertexAttribPointer(phong_shader_p->APosition, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr);
  glEnableVertexAttribArray(phong_shader_p->APosition);
  glVertexAttribPointer(phong_shader_p->ANormal, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));
  glEnableVertexAttribArray(phong_shader_p->ANormal);
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // uploading vertex normal line data
  glGenVertexArrays(1, &vert_norm_line_vao);
  glGenBuffers(1, &vert_norm_line_vbo);
  glBindVertexArray(vert_norm_line_vao);
  glBindBuffer(GL_ARRAY_BUFFER, vert_norm_line_vbo);
  glBufferData(GL_ARRAY_BUFFER, mesh.VertexNormalLineSizeBytes(), mesh.VertexNormalLineData(), GL_STATIC_DRAW);
  glVertexAttribPointer(line_shader_p->APosition, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
  glEnableVertexAttribArray(line_shader_p->APosition);
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  try
  {
    GLenum gl_error = glGetError();
    OPENGLERRORCHECK("main.cpp", "Initialize()", "During initialization", gl_error)
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
  int wheel_motion = Input::MouseWheelMotion();
  cur_scale += scale_speed * Time::DT() * wheel_motion;
  if (Input::MouseButtonDown(LEFT)) {
    std::pair<int, int> mouse_motion = Input::MouseMotion();

    if (mouse_motion.first != 0) {
      Math::Vector3 rotate_vector(0.0f, 1.0f, 0.0f);
      float theta = mouse_motion.first / 100.0f;
      Math::Matrix4 new_rotation;
      new_rotation.Rotate(rotate_vector, theta);
      rotation = new_rotation * rotation;
    }
    if (mouse_motion.second != 0) {
      Math::Vector3 rotate_vector(1.0f, 0.0f, 0.0f);
      float theta = mouse_motion.second / 100.0f;
      Math::Matrix4 new_rotation;
      new_rotation.Rotate(rotate_vector, theta);
      rotation = new_rotation * rotation;
    }
  }
  if(Input::KeyPressed(SPACE))
    line_mode = !line_mode;
}

inline void Update()
{
  ImGuiIO & imgui_io = ImGui::GetIO();
  if(!imgui_io.WantCaptureMouse)
    ManageInput();
}

inline void Render()
{
  Math::Matrix4 translation;
  translation.Translate(cur_tran.x, cur_tran.y, cur_tran.z);
  Math::Matrix4 scale;
  scale.Scale(cur_scale, cur_scale, cur_scale);
  Math::Matrix4 trans_rot_scale_model = translation * rotation * scale;
  Math::Matrix4 projection = Math::Matrix4::Perspective(PI / 2.0f, OpenGLContext::AspectRatio(), 0.1f, 100.0f);
  // drawing object
  phong_shader_p->Use();
  glUniformMatrix4fv(phong_shader_p->UProjection, 1, GL_TRUE, projection.array);
  glUniformMatrix4fv(phong_shader_p->UModel, 1, GL_TRUE, trans_rot_scale_model.array);
  glBindVertexArray(mesh_vao);
  if (line_mode)
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  else
    glPolygonMode(GL_FRONT, GL_FILL);
  glDrawElements(GL_TRIANGLES, num_elements, GL_UNSIGNED_INT, nullptr);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glBindVertexArray(0);

  // drawing lines
  if(show_vertn){
    line_shader_p->Use();
    glUniformMatrix4fv(line_shader_p->UProjection, 1, GL_TRUE, projection.array);
    glUniformMatrix4fv(line_shader_p->UModel, 1, GL_TRUE, trans_rot_scale_model.array);
    glBindVertexArray(vert_norm_line_vao);
    glDrawArrays(GL_LINES, 0, mesh_p->Temp());
    glBindVertexArray(0);
  }
  try
  {
    GLenum gl_error = glGetError();
    OPENGLERRORCHECK("main.cpp", "Update()", "During Update", gl_error)
  }
  catch (const Error & error)
  {
    ErrorLog::Write(error);
  }
}

inline void End()
{
  phong_shader_p->Purge();
  // make sure to free up other shit
  delete phong_shader_p;
}

// some old input code that I don't want
/*if (Input::KeyDown(W))
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
cur_scale -= Time::DT() * 10.0f;*/

/* keeping just in case
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
3, 6, 2
};
*/