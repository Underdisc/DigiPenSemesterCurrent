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

#include "Graphics\Mesh\Mesh.h"
#include "Graphics\Mesh\MeshRenderer.h"
#include "Graphics\ShaderLibrary.h"

#include "Core\Input.h"

#include "Graphics\Texture.h"


#define PI 3.141592653589f
#define MODEL_PATH "Resource/Model/"
#define FILENAME_BUFFERSIZE 50

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

Mesh * mesh = nullptr;
unsigned int mesh_id = -1;
std::string current_mesh("cube.obj");
char next_mesh[FILENAME_BUFFERSIZE] = "cube.obj";
Light light(Math::Vector3(0.0f, 0.0f, 0.0f), 0.5f, 0.4f, 0.5f, 20.0f,
  Color(1.0f, 1.0f, 1.0f), Color(1.0f, 1.0f, 1.0f),
  Color(0.8f, 0.6f, 0.0f));

Color vertex_normal_color(0.0f, 0.0f, 0.0f);
Color face_normal_color(0.0f, 0.0f, 0.0f);


Math::Vector3 trans(0.0f, 0.0f, -2.0f);
float cur_scale = 1.0f;
float scale_speed = 7.0f;
// initializing to pie so rotations make sense at the start
Math::EulerAngles rotate(0.0f, PI, 0.0f, Math::EulerOrders::XYZs);


// GLOBAL

void LoadMesh(const std::string & model);
void Update();
void EditorUpdate();
void Render();


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
  MeshRenderer::Initialize();
  ImGui_ImplSdlGL3_Init(SDLContext::SDLWindow());
  SDLContext::AddEventProcessor(ImGui_ImplSdlGL3_ProcessEvent);

  LoadMesh(current_mesh);
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

  MeshRenderer::Purge();
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

    MeshRenderer::MeshObject * mesh_object = MeshRenderer::GetMeshObject(mesh_id);
    ImGui::Text("Load Different Mesh");
    ImGui::InputText("", next_mesh, FILENAME_BUFFERSIZE);
    if (ImGui::Button("Load Mesh"))
      LoadMesh(next_mesh);
    ImGui::Separator();
    ImGui::Text("Mesh Stats");
    ImGui::Text("Current Mesh: %s", current_mesh.c_str());
    ImGui::Text("Vertex Count: %d", mesh->VertexCount());
    ImGui::Text("Face Count: %d", mesh->FaceCount());
    ImGui::Separator();
    ImGui::Text("Color");
    ImGui::ColorEdit3("Mesh Color", mesh_object->_color._values);
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
    ImGui::ColorEdit3("Vertex Normal Color", mesh_object->_vertexNormalColor._values);
    ImGui::ColorEdit3("Face Normal Color", mesh_object->_faceNormalColor._values);
    ImGui::Checkbox("Show Vertex Normals", &mesh_object->_showVertexNormals);
    ImGui::Checkbox("Show Face Normals", &mesh_object->_showFaceNormals);
    ImGui::Separator();
    ImGui::Text("Other");
    ImGui::Checkbox("Wireframe", &mesh_object->_showWireframe);
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
    ImGui::ColorEdit3("Ambient Color", light._ambientColor._values);
    ImGui::ColorEdit3("Diffuse Color", light._diffuseColor._values);
    ImGui::ColorEdit3("Specular Color", light._specularColor._values);
    ImGui::Separator();
  }
  ImGui::End();
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
    MeshRenderer::Purge(mesh_id);
    Mesh::Purge(mesh);
  }
  // uploading mesh data to gpu
  new_mesh->SetNormalLineLengthMeshRelative(0.1f);
  mesh_id = MeshRenderer::Upload(new_mesh);
  // new mesh loaded
  mesh = new_mesh;
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
  //if(Input::KeyPressed(SPACE))
    //wireframe_mode = !wireframe_mode;
}

inline void Update()
{
  ImGuiIO & imgui_io = ImGui::GetIO();
  if(!imgui_io.WantCaptureMouse)
    ManageInput();
}

inline void Render()
{
  Math::Matrix4 projection;
  Math::Matrix4 view;
  Math::Matrix4 model;

  Math::Matrix4 translation;
  translation.Translate(trans.x, trans.y, trans.z);
  Math::Matrix4 scale;
  scale.Scale(cur_scale, cur_scale, cur_scale);
  Math::Matrix4 rotation;
  Math::ToMatrix4(rotate, &rotation);

  projection = Math::Matrix4::Perspective(PI / 2.0f, OpenGLContext::AspectRatio(), 0.1f, 100.0f);
  view.SetIdentity();
  model = translation * rotation * scale;

  // drawing object
  PhongShader * phong_shader = MeshRenderer::GetPhongShader();
  LineShader * line_shader = MeshRenderer::GetLineShader();
  phong_shader->Use();
  light.SetUniforms(phong_shader);

  // rendering mesh
  MeshRenderer::Render(mesh_id, projection, view, model);
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