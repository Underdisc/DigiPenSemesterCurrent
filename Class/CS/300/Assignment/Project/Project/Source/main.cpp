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

#include "External\Imgui\imgui.h"
#include "External\Imgui\imgui_impl_sdl_gl3.h"

#include "Graphics\SDLContext.h"
#include <GL\glew.h>
#include "Utility\OpenGLError.h"

#include "Graphics\OpenGLContext.h"
#include "Core\Time.h"
#include "Math\Matrix4.h"
#include "Math\Vector3.h"
#include "Math\EulerAngles.h"
#include "Math\MathFunctions.h"

#include "Graphics\Mesh.h"
#include "Graphics\ShaderLibrary.h"

#include "Core\Input.h"

#include "Graphics\Texture.h"


#define PI 3.141592653589f
#define MODEL_PATH "Resource/Model/"
#define FILENAME_BUFFERSIZE 50

// defining a color
struct Color
{
  Color(float x, float y, float z) : _x(x), _y(y), _z(z) {}
  Color(const Color & other) : _x(other._x), _y(other._y), _z(other._z) {}
  union {
    float values[3];
    struct {
      float _x, _y, _z;
    };
  };
};

// defining a light
struct Light
{
  Light(const Math::Vector3 & position, float ambient_factor, 
    float diffuse_factor, float specular_factor, float specular_exponent,
    const Color & ambient_color, const Color & diffuse_color,
    const Color & specular_color) :
    _position(position), _ambientFactor(ambient_factor), 
    _diffuseFactor(diffuse_factor), _specularFactor(specular_factor), 
    _specularExponent(specular_exponent), _ambientColor(ambient_color),
    _diffuseColor(diffuse_color), _specularColor(specular_color)
  {}
  Math::Vector3 _position;
  float _ambientFactor;
  float _diffuseFactor;
  float _specularFactor;
  float _specularExponent;
  Color _ambientColor;
  Color _diffuseColor;
  Color _specularColor;
  void SetUniforms(PhongShader * phong_shader)
  {
    glUniform3f(phong_shader->ULightPosition, 
      _position.x, _position.y, _position.z);
    glUniform1f(phong_shader->UAmbientFactor, _ambientFactor);
    glUniform1f(phong_shader->UDiffuseFactor, _diffuseFactor);
    glUniform1f(phong_shader->USpecularFactor, _specularFactor);
    glUniform1f(phong_shader->USpecularExponent, _specularExponent);
    glUniform3f(phong_shader->UAmbientColor,
      _ambientColor._x, _ambientColor._y, _ambientColor._z);
    glUniform3f(phong_shader->UDiffuseColor,
      _diffuseColor._x, _diffuseColor._y, _diffuseColor._z);
    glUniform3f(phong_shader->USpecularColor,
      _specularColor._x, _specularColor._y, _specularColor._z);

  }
};


// GLOBAL

float clear_color[]{ 0.4f, 0.4f, 0.4f };

PhongShader * phong_shader_p = nullptr;
LineShader * line_shader_p = nullptr;
Mesh * mesh_p = nullptr;
std::string current_mesh("cube.obj");
char next_mesh[FILENAME_BUFFERSIZE] = "cube.obj";
Light light(Math::Vector3(0.0f, 0.0f, 0.0f), 0.5f, 0.4f, 0.5f, 20.0f,
  Color(1.0f, 1.0f, 1.0f), Color(1.0f, 1.0f, 1.0f),
  Color(0.8f, 0.6f, 0.0f));

Color vertex_normal_color(0.0f, 0.0f, 0.0f);
Color face_normal_color(0.0f, 0.0f, 0.0f);

GLuint mesh_vbo;
GLuint mesh_ebo;
GLuint mesh_vao;

GLuint vert_norm_line_vbo;
GLuint vert_norm_line_vao;

GLuint face_norm_line_vbo;
GLuint face_norm_line_vao;


Math::Vector3 trans(0.0f, 0.0f, -2.0f);
float cur_scale = 1.0f;
float scale_speed = 7.0f;
// initializing to pie so rotations make sense at the start
Math::EulerAngles rotate(0.0f, PI, 0.0f, Math::EulerOrders::XYZs);

bool wireframe_mode = false;
unsigned num_elements;

bool show_facen = false;
bool show_vertn = false;
// GLOBAL

void Initialize();
void LoadMesh(const std::string & model);
void UnloadMesh(Mesh * mesh);
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

int main(int argc, char * argv[])
{
  ErrorLog::Clean();
  SDLContext::Create("CS 300 - Assignment 1", true, OpenGLContext::AdjustViewport);
  OpenGLContext::Initialize();

  // temp
  ImGui_ImplSdlGL3_Init(SDLContext::SDLWindow());
  SDLContext::AddEventProcessor(ImGui_ImplSdlGL3_ProcessEvent);
  Initialize();
  // temp

  /*
  TextureShader * test_shader = new TextureShader();

  Texture texture("Resource/Texture/midair.png");

  GLuint test_vao;
  GLuint test_vbo;
  GLuint test_ebo;

  // texture test
  GLfloat vertex[] {
  -0.5f, -0.5f, -0.5f,     0.0f, 0.0f,
  -0.5f,  0.5f, -0.5f,     0.0f, 1.0f,
   0.5f, -0.5f, -0.5f,     1.0f, 0.0f,
   0.5f,  0.5f, -0.5f,     1.0f, 1.0f
  };

  GLuint element[]{
    0, 3, 1,
    0, 2, 3
  };

  
  glGenVertexArrays(1, &test_vao);
  glGenBuffers(1, &test_vbo);
  glGenBuffers(1, &test_ebo);

  glBindVertexArray(test_vao);

  glBindBuffer(GL_ARRAY_BUFFER, test_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, test_ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(element), element, GL_STATIC_DRAW);

  glVertexAttribPointer(test_shader->APosition, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
  glEnableVertexAttribArray(test_shader->APosition);

  glVertexAttribPointer(test_shader->ATexCoord, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));
  glEnableVertexAttribArray(test_shader->ATexCoord);
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  */





  glEnable(GL_DEPTH_TEST);
  while (SDLContext::KeepOpen())
  {
    InitialUpdate();
    Update();
    EditorUpdate();
    glClearColor(clear_color[0], clear_color[1], clear_color[2], 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /*texture.Bind();
    test_shader->Use();
    glBindVertexArray(test_vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    Texture::Unbind();
    glBindVertexArray(0);*/
    Render();
    ImGui::Render();
    OpenGLContext::Swap();
  }

  //End();

  //delete test_shader;

  OpenGLContext::Purge();
  SDLContext::Purge();
}

inline void EditorUpdate()
{
  ImGui::Begin("Editor");
  if (ImGui::CollapsingHeader("Help"))
  {
    ImGui::TextWrapped("Hello! There are a few controls I would like to "
      "mention so you know how to use this application.");
    ImGui::Separator();
    if (ImGui::TreeNode("QOL Controls"))
    {
      ImGui::Separator();
      ImGui::TextWrapped("Click and drag anywhere outside the ImGui "
        "window to rotate the object.");
      ImGui::Separator();
      ImGui::TextWrapped("Use the mouse scroll wheel to perform a "
        "uniform scaling on the object.");
      ImGui::Separator();
      ImGui::TextWrapped("Use Spacebar to switch between face and "
        "wireframe mode.");
      ImGui::Separator();
      ImGui::TextWrapped("These will not work when the mouse is hovering over "
        "the ImGui window.");
      ImGui::Separator();
      ImGui::TreePop();
    }
    ImGui::Separator();
    if (ImGui::TreeNode("Editor Tabs")) {
      ImGui::Separator();
      ImGui::TextWrapped("The Debug tab only contains FPS at the moment.");
      ImGui::Separator();
      ImGui::TextWrapped("The Global window contains parameters for the "
        "Clear Color and Time Scale (not currently in use).");
      ImGui::Separator();
      ImGui::TextWrapped("The Mesh window allows you to edit mesh properites, "
        "display normal lines, and load new meshes.");
      ImGui::Separator();
      ImGui::TextWrapped("The Light window contains parameters for the single "
        "light that is currently being used.");
      ImGui::TreePop();
    }
    ImGui::Separator();
  }
  if (ImGui::CollapsingHeader("Debug")) {
    ImGui::Text("FPS: %f", 1.0f / Time::DT());
    ImGui::Separator();
  }
  if (ImGui::CollapsingHeader("Global")) {
    ImGui::ColorEdit3("Clear Color", clear_color);
    ImGui::DragFloat("Time Scale", &Time::m_TimeScale, 0.01f);
    ImGui::Separator();
  }
  if (ImGui::CollapsingHeader("Mesh")) {
    ImGui::Text("Load Different Mesh");
    ImGui::InputText("", next_mesh, FILENAME_BUFFERSIZE);
    if (ImGui::Button("Load Mesh"))
      LoadMesh(next_mesh);
    ImGui::Separator();
    ImGui::Text("Mesh Stats");
    ImGui::Text("Current Mesh: %s", current_mesh.c_str());
    ImGui::Text("Vertex Count: %d", mesh_p->VertexCount());
    ImGui::Text("Face Count: %d", mesh_p->FaceCount());
    ImGui::Separator();
    ImGui::Text("Translation");
    ImGui::DragFloat("TX", &trans.x, 0.01f);
    ImGui::DragFloat("TY", &trans.y, 0.01f);
    ImGui::DragFloat("TZ", &trans.z, 0.01f);
    ImGui::Separator();
    ImGui::Text("Rotation");
    ImGui::DragFloat("RX", &rotate.Angles.x, 0.01f);
    ImGui::DragFloat("RY", &rotate.Angles.y, 0.01f);
    ImGui::DragFloat("RZ", &rotate.Angles.z, 0.01f);
    ImGui::Separator();
    ImGui::Text("Scale");
    ImGui::DragFloat("s", &cur_scale, 0.01f);
    ImGui::Separator();
    ImGui::Text("Normals");
    ImGui::ColorEdit3("Vertex Normal Color", vertex_normal_color.values);
    ImGui::ColorEdit3("Face Normal Color", face_normal_color.values);
    ImGui::Checkbox("Show Vertex Normals", &show_vertn);
    ImGui::Checkbox("Show Face Normals", &show_facen);
    ImGui::Separator();
    ImGui::Text("Other");
    ImGui::Checkbox("Wireframe", &wireframe_mode);
    ImGui::Separator();
  }
  if (ImGui::CollapsingHeader("Light")) {
    ImGui::Text("Position");
    ImGui::DragFloat("X", &light._position.x, 0.01f);
    ImGui::DragFloat("Y", &light._position.y, 0.01f);
    ImGui::DragFloat("Z", &light._position.z, 0.01f);
    ImGui::Separator();
    ImGui::Text("Light Properties");
    ImGui::SliderFloat("Ambient Factor", &light._ambientFactor, 0.0f, 1.0f);
    ImGui::SliderFloat("Diffuse Factor", &light._diffuseFactor, 0.0f, 1.0f);
    ImGui::SliderFloat("Specular Factor", &light._specularFactor, 0.0f, 1.0f);
    ImGui::SliderFloat("Specular Exponent", &light._specularExponent, 0.0f, 50.0f);
    ImGui::ColorEdit3("Ambient Color", light._ambientColor.values);
    ImGui::ColorEdit3("Diffuse Color", light._diffuseColor.values);
    ImGui::ColorEdit3("Specular Color", light._specularColor.values);
    ImGui::Separator();
  }
  ImGui::End();
}

void LoadMesh(const std::string & model)
{
  Mesh * old_mesh = mesh_p;
  std::string mesh_path = MODEL_PATH + model;
  try {
    mesh_p = new Mesh(mesh_path, Mesh::OBJ);
  }
  catch (const Error & error) {
    return;
  }
  if (old_mesh) 
    UnloadMesh(old_mesh);

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

  mesh_p->SetNormalLineLengthMeshRelative(0.1f);

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

  // uploading face normal line data
  glGenVertexArrays(1, &face_norm_line_vao);
  glGenBuffers(1, &face_norm_line_vbo);
  glBindVertexArray(face_norm_line_vao);
  glBindBuffer(GL_ARRAY_BUFFER, face_norm_line_vbo);
  glBufferData(GL_ARRAY_BUFFER, mesh.FaceNormalLineSizeBytes(), mesh.FaceNormalLineData(), GL_STATIC_DRAW);
  glVertexAttribPointer(line_shader_p->APosition, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
  glEnableVertexAttribArray(line_shader_p->APosition);
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  current_mesh = model;

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

void UnloadMesh(Mesh * mesh)
{
  // deleting gpu mesh data
  glDeleteBuffers(1, &mesh_vbo);
  glDeleteBuffers(1, &mesh_ebo);
  glDeleteVertexArrays(1, &mesh_vao);
  // deleting gpu normal line data
  glDeleteBuffers(1, &vert_norm_line_vbo);
  glDeleteVertexArrays(1, &vert_norm_line_vao);
  glDeleteBuffers(1, &face_norm_line_vbo);
  glDeleteVertexArrays(1, &face_norm_line_vao);
  // deallocate mesh
  delete mesh;
}

inline void Initialize()
{
  phong_shader_p = new PhongShader();
  line_shader_p = new LineShader();
  LoadMesh(current_mesh);
}

inline void ManageInput()
{
  int wheel_motion = Input::MouseWheelMotion();
  cur_scale += scale_speed * Time::DT() * wheel_motion;
  if (Input::MouseButtonDown(LEFT)) {
    std::pair<int, int> mouse_motion = Input::MouseMotion();

    if (mouse_motion.first != 0)
      rotate.Angles.y += mouse_motion.first / 100.0f;
    if (mouse_motion.second != 0) {
      rotate.Angles.x -= mouse_motion.second / 100.0f;
    }
  }
  if(Input::KeyPressed(SPACE))
    wireframe_mode = !wireframe_mode;
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
  translation.Translate(trans.x, trans.y, trans.z);
  Math::Matrix4 scale;
  scale.Scale(cur_scale, cur_scale, cur_scale);
  Math::Matrix4 rotation;
  Math::ToMatrix4(rotate, &rotation);
  Math::Matrix4 trans_rot_scale_model = translation * rotation * scale;
  Math::Matrix4 projection = Math::Matrix4::Perspective(PI / 2.0f, OpenGLContext::AspectRatio(), 0.1f, 100.0f);
  // drawing object
  phong_shader_p->Use();
  light.SetUniforms(phong_shader_p);
  glUniformMatrix4fv(phong_shader_p->UProjection, 1, GL_TRUE, projection.array);
  glUniformMatrix4fv(phong_shader_p->UModel, 1, GL_TRUE, trans_rot_scale_model.array);
  glBindVertexArray(mesh_vao);
  if (wireframe_mode)
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  else
    glPolygonMode(GL_FRONT, GL_FILL);
  glDrawElements(GL_TRIANGLES, num_elements, GL_UNSIGNED_INT, nullptr);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glBindVertexArray(0);

  // drawing lines
  line_shader_p->Use();
  if(show_vertn){
    glUniform3f(line_shader_p->ULineColor,
      vertex_normal_color._x, vertex_normal_color._y, vertex_normal_color._z);
    glUniform1f(line_shader_p->UTime, Time::TotalTimeScaled());

    glUniformMatrix4fv(line_shader_p->UProjection, 1, GL_TRUE, projection.array);
    glUniformMatrix4fv(line_shader_p->UModel, 1, GL_TRUE, trans_rot_scale_model.array);
    glBindVertexArray(vert_norm_line_vao);
    glDrawArrays(GL_LINES, 0, mesh_p->VertexNormalLineSizeVertices());
    glBindVertexArray(0);
  }
  if (show_facen) {
    glUniform3f(line_shader_p->ULineColor,
      face_normal_color._x, face_normal_color._y, face_normal_color._z);
    glUniform1f(line_shader_p->UTime, Time::TotalTimeScaled());

    glUniformMatrix4fv(line_shader_p->UProjection, 1, GL_TRUE, projection.array);
    glUniformMatrix4fv(line_shader_p->UModel, 1, GL_TRUE, trans_rot_scale_model.array);
    glBindVertexArray(face_norm_line_vao);
    glDrawArrays(GL_LINES, 0, mesh_p->FaceNormalLineSizeVertices());
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
  UnloadMesh(mesh_p);
  phong_shader_p->Purge();
  line_shader_p->Purge();
  delete line_shader_p;
  delete phong_shader_p;
}