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
#include "Graphics\Camera.h"

#include "Core\Input.h"
#include "Core\Framer.h"
#include "Graphics\Texture.h"

#define PI 3.141592653589f
#define PI2 6.28318530718f
#define FPS 60
#define MODEL_PATH "Resource/Model/"
#define FILENAME_BUFFERSIZE 50

struct Material
{
  Material() : _color(1.0f, 1.0f, 1.0f), _ambientFactor(0.1f), 
    _diffuseFactor(1.0f), _specularFactor(1.0f), _specularExponent(3.0f)
  {}
  void SetUniforms(PhongShader * phong_shader)
  {
    glUniform3f(phong_shader->UMaterial.UColor, 
      _color._r, _color._g, _color._b);
    glUniform1f(phong_shader->UMaterial.UAmbientFactor, _ambientFactor);
    glUniform1f(phong_shader->UMaterial.UDiffuseFactor, _diffuseFactor);
    glUniform1f(phong_shader->UMaterial.USpecularFactor, _specularFactor);
    glUniform1f(phong_shader->UMaterial.USpecularExponent, _specularExponent);
  }
  void SetUniforms(GouraudShader * gouraud_shader)
  {
    glUniform3f(gouraud_shader->UMaterial.UColor,
      _color._r, _color._g, _color._b);
    glUniform1f(gouraud_shader->UMaterial.UAmbientFactor, _ambientFactor);
    glUniform1f(gouraud_shader->UMaterial.UDiffuseFactor, _diffuseFactor);
    glUniform1f(gouraud_shader->UMaterial.USpecularFactor, _specularFactor);
    glUniform1f(gouraud_shader->UMaterial.USpecularExponent, _specularExponent);
  }
  void SetUniforms(BlinnShader * blinn_shader)
  {
    glUniform3f(blinn_shader->UMaterial.UColor,
      _color._r, _color._g, _color._b);
    glUniform1f(blinn_shader->UMaterial.UAmbientFactor, _ambientFactor);
    glUniform1f(blinn_shader->UMaterial.UDiffuseFactor, _diffuseFactor);
    glUniform1f(blinn_shader->UMaterial.USpecularFactor, _specularFactor);
    glUniform1f(blinn_shader->UMaterial.USpecularExponent, _specularExponent);
  }
  Color _color;
  float _ambientFactor;
  float _diffuseFactor;
  float _specularFactor;
  float _specularExponent;
};


// defining a light
struct Light
{
  Light(): _type(0), _position(0.0f, 2.0f, 0.0f), 
    _direction(0.0f, -1.0f, 0.0f), _innerAngle(PI / 12.0f), 
    _outerAngle(PI / 6.0f), _spotExponent(1.0f), 
    _ambientColor(0.0f, 0.0f, 0.0f), _diffuseColor(0.8f, 0.8f, 0.8f),
    _specularColor(1.0f, 1.0f, 1.0f), _attenuationC0(1.0f),
    _attenuationC1(0.1f), _attenuationC2(0.0f)
  {}
  Light(unsigned int type, const Math::Vector3 & position, 
    const Math::Vector3 & direction, const Color & ambient_color,
    const Color & diffuse_color, const Color & specular_color) :
    _type(type), _position(position), _direction(direction),
    _ambientColor(ambient_color), _diffuseColor(diffuse_color),
    _specularColor(specular_color)
  {}
  int _type;
  Math::Vector3 _position;
  Math::Vector3 _direction;
  float _innerAngle;
  float _outerAngle;
  float _spotExponent;
  Color _ambientColor;
  Color _diffuseColor;
  Color _specularColor;
  float _attenuationC0;
  float _attenuationC1;
  float _attenuationC2;

  static const int _typePoint;
  static const int _typeDirectional;
  static const int _typeSpot;

  static int _activeLights;
  void SetUniforms(unsigned int light_index, PhongShader * phong_shader)
  {
    glUniform1i(phong_shader->ULights[light_index].UType, _type);
    glUniform3f(phong_shader->ULights[light_index].UPosition,
      _position.x, _position.y, _position.z);
    glUniform3f(phong_shader->ULights[light_index].UDirection,
      _direction.x, _direction.y, _direction.z);
    glUniform1f(phong_shader->ULights[light_index].UInnerAngle,
      _innerAngle);
    glUniform1f(phong_shader->ULights[light_index].UOuterAngle,
      _outerAngle);
    glUniform1f(phong_shader->ULights[light_index].USpotExponent,
      _spotExponent);
    glUniform3f(phong_shader->ULights[light_index].UAmbientColor,
      _ambientColor._x, _ambientColor._y, _ambientColor._z);
    glUniform3f(phong_shader->ULights[light_index].UDiffuseColor,
      _diffuseColor._x, _diffuseColor._y, _diffuseColor._z);
    glUniform3f(phong_shader->ULights[light_index].USpecularColor,
      _specularColor._x, _specularColor._y, _specularColor._z);
    glUniform1f(phong_shader->ULights[light_index].UAttenuationC0, 
      _attenuationC0);
    glUniform1f(phong_shader->ULights[light_index].UAttenuationC1,
      _attenuationC1);
    glUniform1f(phong_shader->ULights[light_index].UAttenuationC2,
      _attenuationC2);
  }
  void SetUniforms(unsigned int light_index, GouraudShader * gouraud_shader)
  {
    glUniform1i(gouraud_shader->ULights[light_index].UType, _type);
    glUniform3f(gouraud_shader->ULights[light_index].UPosition,
      _position.x, _position.y, _position.z);
    glUniform3f(gouraud_shader->ULights[light_index].UDirection,
      _direction.x, _direction.y, _direction.z);
    glUniform1f(gouraud_shader->ULights[light_index].UInnerAngle,
      _innerAngle);
    glUniform1f(gouraud_shader->ULights[light_index].UOuterAngle,
      _outerAngle);
    glUniform1f(gouraud_shader->ULights[light_index].USpotExponent,
      _spotExponent);
    glUniform3f(gouraud_shader->ULights[light_index].UAmbientColor,
      _ambientColor._x, _ambientColor._y, _ambientColor._z);
    glUniform3f(gouraud_shader->ULights[light_index].UDiffuseColor,
      _diffuseColor._x, _diffuseColor._y, _diffuseColor._z);
    glUniform3f(gouraud_shader->ULights[light_index].USpecularColor,
      _specularColor._x, _specularColor._y, _specularColor._z);
    glUniform1f(gouraud_shader->ULights[light_index].UAttenuationC0,
      _attenuationC0);
    glUniform1f(gouraud_shader->ULights[light_index].UAttenuationC1,
      _attenuationC1);
    glUniform1f(gouraud_shader->ULights[light_index].UAttenuationC2,
      _attenuationC2);
  }
  void SetUniforms(unsigned int light_index, BlinnShader * blinn_shader)
  {
    glUniform1i(blinn_shader->ULights[light_index].UType, _type);
    glUniform3f(blinn_shader->ULights[light_index].UPosition,
      _position.x, _position.y, _position.z);
    glUniform3f(blinn_shader->ULights[light_index].UDirection,
      _direction.x, _direction.y, _direction.z);
    glUniform1f(blinn_shader->ULights[light_index].UInnerAngle,
      _innerAngle);
    glUniform1f(blinn_shader->ULights[light_index].UOuterAngle,
      _outerAngle);
    glUniform1f(blinn_shader->ULights[light_index].USpotExponent,
      _spotExponent);
    glUniform3f(blinn_shader->ULights[light_index].UAmbientColor,
      _ambientColor._x, _ambientColor._y, _ambientColor._z);
    glUniform3f(blinn_shader->ULights[light_index].UDiffuseColor,
      _diffuseColor._x, _diffuseColor._y, _diffuseColor._z);
    glUniform3f(blinn_shader->ULights[light_index].USpecularColor,
      _specularColor._x, _specularColor._y, _specularColor._z);
    glUniform1f(blinn_shader->ULights[light_index].UAttenuationC0,
      _attenuationC0);
    glUniform1f(blinn_shader->ULights[light_index].UAttenuationC1,
      _attenuationC1);
    glUniform1f(blinn_shader->ULights[light_index].UAttenuationC2,
      _attenuationC2);
  }
};
int Light::_activeLights = 1;
const int Light::_typePoint = 0;
const int Light::_typeDirectional = 1;
const int Light::_typeSpot = 2;



// GLOBAL

//editor
bool show_light_editor = false;
bool show_material_editor = false;

Mesh * mesh = nullptr;
unsigned int mesh_id = -1;
unsigned int sphere_mesh_id = -1;
unsigned int plane_mesh_id = -1;
std::string current_mesh("bunny.obj");
char next_mesh[FILENAME_BUFFERSIZE] = "bunny.obj";
Light lights[MAXLIGHTS];
Material material;
unsigned int active_lights = 2;
MeshRenderer::ShaderType shader_in_use = MeshRenderer::PHONG;
bool rotating_lights = false;
float rotate_light_speed = 1.5f;


bool rotate_camera = false;
float camera_rotate_speed = 1.0f;
float camera_distance = 2.0f;


Math::Vector3 trans(0.0f, 0.0f, 0.0f);
float cur_scale = 1.0f;
float scale_speed = 7.0f;
// initializing to pie so rotations make sense at the start
Math::EulerAngles rotation(0.0f, PI, 0.0f, Math::EulerOrders::XYZs);


Camera camera(Math::Vector3(0.0f, 1.0f, 0.0f));
float movespeed = 1.0f;
float mouse_sensitivity = 0.3f;
float mouse_wheel_sensitivity = 2.0f;

// GLOBAL

void LoadMesh(const std::string & model);
void Update();
void EditorUpdate();
void Render();

void LoadOtherMeshes()
{
  Mesh sphere_mesh(MODEL_PATH + std::string("sphere.obj"), Mesh::OBJ);
  sphere_mesh_id = MeshRenderer::Upload(&sphere_mesh);
  Mesh plane_mesh(MODEL_PATH + std::string("plane.obj"), Mesh::OBJ);
  plane_mesh_id = MeshRenderer::Upload(&plane_mesh);
}

inline void InitialUpdate()
{
  Time::Update();
  ImGui_ImplSdlGL3_NewFrame(SDLContext::SDLWindow());
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
  ImGui_ImplSdlGL3_Init(SDLContext::SDLWindow());
  SDLContext::AddEventProcessor(ImGui_ImplSdlGL3_ProcessEvent);

  LoadMesh(current_mesh);
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
    EditorUpdate();
    Update();
    Clear();
    Render();
    ImGui::Render();
    OpenGLContext::Swap();
    // frame end
    Framer::End();
  }

  Mesh::Purge(mesh);
  MeshRenderer::Purge();
  OpenGLContext::Purge();
  SDLContext::Purge();
}

inline void SceneSame()
{
  Light::_activeLights = 2;
  
  lights[0]._position = Math::Vector3(-2.5f, 0.0f, -1.0f);
  lights[1]._position = Math::Vector3(1.0f, 1.0f, 0.0f);

  for(int i = 0; i < Light::_activeLights; ++i) {
    lights[i]._type = Light::_typePoint;
    lights[i]._attenuationC0 = 1.0f;
    lights[i]._attenuationC1 = 0.1f;
    lights[i]._attenuationC2 = 0.0f;
    lights[i]._ambientColor = Color(0.0f, 0.4f, 0.0f);
    lights[i]._diffuseColor = Color(0.2f, 0.4f, 0.4f);
    lights[i]._specularColor = Color(1.0f, 1.0f, 1.0f);
  }
}

inline void SceneMix()
{
  Light::_activeLights = 3;
  lights[0]._type = Light::_typeDirectional;
  lights[0]._position = Math::Vector3(-5.0f, 0.0f, 0.0f);
  lights[0]._direction = Math::Vector3(1.0f, -1.0f, 0.0f);
  lights[0]._ambientColor = Color(0.0f, 0.0f, 0.4f);
  lights[0]._diffuseColor = Color(0.23f, 0.2f, 0.23f);
  lights[0]._specularColor = Color(1.0f, 0.8f, 0.53f);

  lights[1]._type = Light::_typePoint;
  lights[1]._position = Math::Vector3(0.0f, -2.0f, 0.0f);
  lights[1]._attenuationC0 = 0.0f;
  lights[1]._attenuationC1 = 0.4f;
  lights[1]._attenuationC2 = 0.4f;
  lights[1]._ambientColor = Color(0.0f, 0.0f, 0.0f);
  lights[1]._diffuseColor = Color(0.0f, 0.8f, 0.53f);
  lights[1]._specularColor = Color(1.0f, 1.0f, 1.0f);

  lights[2]._type = Light::_typeSpot;
  lights[2]._position = Math::Vector3(1.9f, 1.7f, -0.8f);
  lights[2]._direction = Math::Vector3(-1.0f, -1.0f, 0.5f);
  lights[2]._innerAngle = 0.027f;
  lights[2]._outerAngle = 0.126f;
  lights[2]._attenuationC0 = 0.0f;
  lights[2]._attenuationC1 = 0.0f;
  lights[2]._attenuationC2 = 0.5f;
  lights[2]._ambientColor = Color(0.0f, 0.0f, 0.0f);
  lights[2]._diffuseColor = Color(0.6f, 0.3f, 0.3f);
  lights[2]._specularColor = Color(0.6f, 0.4f, 0);

  
}

inline void MaterialEditor()
{
  ImGui::Begin("Material", &show_material_editor);
  ImGui::ColorEdit3("Color", material._color._values);
  ImGui::SliderFloat("Ambient Factor", &material._ambientFactor, 0.0f, 1.0f);
  ImGui::SliderFloat("Diffuse Factor", &material._diffuseFactor, 0.0f, 1.0f);
  ImGui::SliderFloat("Specular Factor", &material._specularFactor, 0.0f, 1.0f);
  ImGui::SliderFloat("Specular Exponent", &material._specularExponent, 0.0f, 30.0f);
  ImGui::End();
}

inline void LightEditor()
{
  ImGui::Begin("Lights", &show_light_editor);
  if(ImGui::CollapsingHeader("Scenes")){
    if(ImGui::Button("Same"))
      SceneSame();
    ImGui::SameLine();
    if(ImGui::Button("Mix"))
      SceneMix();
    ImGui::Separator();
    ImGui::Checkbox("Rotate Lights", &rotating_lights);
    ImGui::DragFloat("Rotation Speed", &rotate_light_speed, 0.01f);
    ImGui::Separator();
  }
  ImGui::InputInt("Active Lights", &Light::_activeLights, 1, 1);
  if(Light::_activeLights > MAXLIGHTS)
    Light::_activeLights = MAXLIGHTS;
  else if(Light::_activeLights < 0)
    Light::_activeLights = 0;

  for(int i = 0; i < Light::_activeLights; ++i){
    std::string light_name("Light" + std::to_string(i));
    if(ImGui::TreeNode(light_name.c_str())){
      ImGui::Combo("Type", &lights[i]._type, "Point\0Directional\0Spot\0\0");
      int type = lights[i]._type;
      if(type == Light::_typePoint || type == Light::_typeSpot){
        ImGui::Text("Position");
        ImGui::DragFloat("PX", &lights[i]._position.x, 0.01f);
        ImGui::DragFloat("PY", &lights[i]._position.y, 0.01f);
        ImGui::DragFloat("PZ", &lights[i]._position.z, 0.01f);
      }
      if (type == Light::_typeDirectional || type == Light::_typeSpot) {
        ImGui::Text("Direction");
        ImGui::DragFloat("DX", &lights[i]._direction.x, 0.01f);
        ImGui::DragFloat("DY", &lights[i]._direction.y, 0.01f);
        ImGui::DragFloat("DZ", &lights[i]._direction.z, 0.01f);
      }
      if (type == Light::_typeSpot) {
        ImGui::Text("Spotlight Properties");
        float * inner_angle = &lights[i]._innerAngle;
        float * outer_angle = &lights[i]._outerAngle;
        ImGui::SliderFloat("Inner", inner_angle, 0.0f, *outer_angle);
        ImGui::SliderFloat("Outer", outer_angle, *inner_angle, PI / 2.0f);
        ImGui::SliderFloat("Exponent", &lights[i]._spotExponent, 0.0f, 30.0f);
      }
      if (type != Light::_typeDirectional) {
        ImGui::Text("Attenuation Coefficients");
        ImGui::SliderFloat("C0", &lights[i]._attenuationC0, 0.0f, 5.0f);
        ImGui::SliderFloat("C1", &lights[i]._attenuationC1, 0.0f, 5.0f);
        ImGui::SliderFloat("C2", &lights[i]._attenuationC2, 0.0f, 5.0f);
      }
      ImGui::Text("Light Colors");
      ImGui::ColorEdit3("Ambient Color", lights[i]._ambientColor._values);
      ImGui::ColorEdit3("Diffuse Color", lights[i]._diffuseColor._values);
      ImGui::ColorEdit3("Specular Color", lights[i]._specularColor._values);
      ImGui::TreePop();
     }
    ImGui::Separator();
  }
  ImGui::End();

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
      ImGui::TextWrapped("Left click and drag anywhere outside the ImGui "
        "window to rotate the object.");
      ImGui::Separator();
      ImGui::TextWrapped("Use WASD to move the camera around world. E will "
        "raise the camera and Q will lower the camera.");
      ImGui::Separator();
      ImGui::TextWrapped("Right click and drag anywhere outside the ImGui "
        "window to rotate the camera.");
      ImGui::Separator();
      ImGui::TextWrapped("Use the mouse scroll wheel to increase and "
        "decrease the movement speed of the camera.");
      ImGui::Separator();
      ImGui::TextWrapped("These will not work when the mouse is hovering over "
        "an ImGui window.");
      ImGui::Separator();
      ImGui::TreePop();
    }
    ImGui::Separator();
    if (ImGui::TreeNode("Editor Tabs")) {
      ImGui::Separator();
      ImGui::TextWrapped("The Debug tab only contains the average FPS and "
        "average frame usage over a single second.");
      ImGui::Separator();
      ImGui::TextWrapped("The Global tab contains parameters for "
        "adjusting global colors (Emissive/Global Ambient/Fog), fog near/far " 
        "planes and time scale");
      ImGui::Separator();
      ImGui::TextWrapped("The Camera tab contains parameters for the "
        "near/far planes of the view frustum. It also contains the rotating "
        "camera option. When rotating the camera after disabling this "
        "option, the camera might get flipped around");
      ImGui::Separator();
      ImGui::TextWrapped("The Mesh tab allows you to edit mesh properites, "
        "display normal lines, and load new meshes.");
      ImGui::Separator();
      ImGui::TextWrapped("The Shader tab contains an option for swapping out "
        "the shader that is used for rendering.");
      ImGui::Separator();
      ImGui::TextWrapped("The Material and Light Editor checkboxes will open "
        "new ImGui windows for editing material and light properties when "
        "selected.");
      ImGui::TreePop();
    }
    ImGui::Separator();
    if (ImGui::TreeNode("Light Editor")){
      ImGui::Separator();
      ImGui::TextWrapped("You can add, remove and adjust light properties in "
        "the light editor. The Scene tab at the top contains the two presets "
        "for lighting the scene and the option to rotate the lights around "
        "the model.");
      ImGui::TreePop();
    }

    ImGui::Separator();
  }
  if (ImGui::CollapsingHeader("Debug")) {
    ImGui::Text("Average FPS: %f", Framer::AverageFPS());
    ImGui::Text("Average Frame Usage: %f", Framer::AverageFrameUsage() * 100.0f);
    ImGui::Separator();
  }
  if (ImGui::CollapsingHeader("Global")) {
    ImGui::Text("Global Colors");
    ImGui::ColorEdit3("Emissive", MeshRenderer::_emissiveColor._values);
    ImGui::ColorEdit3("Global Amibent", 
      MeshRenderer::_globalAmbientColor._values);
    ImGui::ColorEdit3("Fog Color", MeshRenderer::_fogColor._values);
    ImGui::Separator();
    ImGui::Text("Fog Properties");
    ImGui::SliderFloat("Fog Near", &MeshRenderer::_fogNear, 0.0f, MeshRenderer::_fogFar);
    ImGui::SliderFloat("Fog Far", &MeshRenderer::_fogFar, MeshRenderer::_fogNear, 100.0f);
    ImGui::Separator();
    ImGui::DragFloat("Time Scale", &Time::m_TimeScale, 0.01f);
    ImGui::Separator();
  }
  if (ImGui::CollapsingHeader("Camera")) {
    ImGui::SliderFloat("Near Plane", &MeshRenderer::_nearPlane, 0.01f, 5.0f);
    ImGui::SliderFloat("Far Plane", &MeshRenderer::_farPlane, MeshRenderer::_nearPlane, 100.0f);
    ImGui::Separator();
    ImGui::Checkbox("Rotating Camera", &rotate_camera);
    ImGui::DragFloat("Rotation Distance", &camera_distance, 0.01f);
    ImGui::DragFloat("Rotation Speed", &camera_rotate_speed, 0.01f);
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
    ImGui::Text("Translation");
    ImGui::DragFloat("TX", &trans.x, 0.01f);
    ImGui::DragFloat("TY", &trans.y, 0.01f);
    ImGui::DragFloat("TZ", &trans.z, 0.01f);
    ImGui::Separator();
    ImGui::Text("Rotation");
    ImGui::DragFloat("RX", &rotation.Angles.x, 0.01f);
    ImGui::DragFloat("RY", &rotation.Angles.y, 0.01f);
    ImGui::DragFloat("RZ", &rotation.Angles.z, 0.01f);
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
  if (ImGui::CollapsingHeader("Shader")) {
    int shader_int = MeshRenderer::ShaderTypeToInt(shader_in_use);
    ImGui::Combo("Shader Type", &shader_int, "Phong\0Gouraud\0Blinn\0\0");
    shader_in_use = MeshRenderer::IntToShaderType(shader_int);
    if (ImGui::Button("Reload Selected Shader"))
      MeshRenderer::ReloadShader(shader_in_use);
    ImGui::Separator();
  }
  ImGui::Checkbox("Show Material Editor", &show_material_editor);
  ImGui::Checkbox("Show Light Editor", &show_light_editor);

  ImGui::End();
  if (show_material_editor)
    MaterialEditor();
  if (show_light_editor)
    LightEditor();
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
  if (Input::MouseButtonDown(LEFT)) {
    std::pair<int, int> mouse_motion = Input::MouseMotion();

    if (mouse_motion.first != 0)
      rotation.Angles.y += mouse_motion.first / 100.0f;
    if (mouse_motion.second != 0) {
      rotation.Angles.x -= mouse_motion.second / 100.0f;
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
  if (Input::MouseButtonDown(RIGHT) && !rotate_camera) {
    std::pair<int, int> mouse_motion = Input::MouseMotion();
    camera.MoveYaw((float)mouse_motion.first * Time::DT() * mouse_sensitivity);
    camera.MovePitch(-(float)mouse_motion.second * Time::DT() * mouse_sensitivity);
  }
    
}

inline void RotateCamera()
{
  float camera_angle = Time::TotalTimeScaled() * camera_rotate_speed;
  Math::Vector3 new_position;
  new_position.x = Math::Cos(camera_angle) * camera_distance;
  new_position.y = Math::Sin(camera_angle);
  new_position.z = Math::Sin(camera_angle) * camera_distance;
  camera.SetPosition(new_position);
  camera.LookAt(Math::Vector3(trans.x, trans.y, trans.z));
}

inline void RotateLights()
{
  for (int i = 0; i < Light::_activeLights; ++i) {
    float percentage = (float)i / (float)Light::_activeLights;
    float light_angle = percentage * PI2 + Time::TotalTimeScaled() * rotate_light_speed;
    Light & light = lights[i];

    float cos_angle = Math::Cos(light_angle);
    float sin_angle = Math::Sin(light_angle);
    light._position.x = cos_angle * cur_scale + 0.5f * cos_angle + trans.x;
    light._position.y = trans.y;
    light._position.z = sin_angle * cur_scale + 0.5f * sin_angle + trans.z;

    light._direction.x = -cos_angle;
    light._direction.y = 0.0f;
    light._direction.z = -sin_angle;
  }


}

inline void Update()
{
  ImGuiIO & imgui_io = ImGui::GetIO();
  if(!imgui_io.WantCaptureMouse)
    ManageInput();
  if(rotating_lights)
    RotateLights();
  if(rotate_camera)
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
    translate.Translate(lights[i]._position.x, lights[i]._position.y, lights[i]._position.z);
    scale.Scale(0.25f, 0.25f, 0.25f);
    model = translate * scale;
    Color & color = lights[i]._diffuseColor;
    glUniform3f(solid_shader->UColor, color._r, color._g, color._b);
    MeshRenderer::Render(sphere_mesh_id, MeshRenderer::SOLID, projection, camera.ViewMatrix(), model);
  }

  translate.Translate(trans.x, trans.y, trans.z);
  scale.Scale(cur_scale, cur_scale, cur_scale);
  Math::ToMatrix4(rotation, &rotate);
  model = translate * rotate * scale;

  const Math::Vector3 & cpos = camera.GetPosition();
  switch (shader_in_use)
  {
  //PHONG SHADER
  case MeshRenderer::ShaderType::PHONG:
    phong_shader->Use();
    glUniform3f(phong_shader->UCameraPosition, cpos.x, cpos.y, cpos.z);
    glUniform1i(phong_shader->UActiveLights, Light::_activeLights);
    for (int i = 0; i < Light::_activeLights; ++i)
      lights[i].SetUniforms(i, phong_shader);
    material.SetUniforms(phong_shader);
    break;
  // GOURAUD SHADER
  case MeshRenderer::ShaderType::GOURAUD:
    gouraud_shader->Use();
    glUniform3f(gouraud_shader->UCameraPosition, cpos.x, cpos.y, cpos.z);
    glUniform1i(gouraud_shader->UActiveLights, Light::_activeLights);
    glUniform1i(gouraud_shader->UActiveLights, Light::_activeLights);
    for (int i = 0; i < Light::_activeLights; ++i)
      lights[i].SetUniforms(i, gouraud_shader);
    material.SetUniforms(gouraud_shader);
    break;
  case MeshRenderer::ShaderType::BLINN:
    blinn_shader->Use();
    glUniform3f(blinn_shader->UCameraPosition, cpos.x, cpos.y, cpos.z);
    glUniform1i(blinn_shader->UActiveLights, Light::_activeLights);
    for (int i = 0; i < Light::_activeLights; ++i)
      lights[i].SetUniforms(i, blinn_shader);
    material.SetUniforms(blinn_shader);
    break;
  default:
    break;
  }
  // rendering mesh
  MeshRenderer::Render(mesh_id, shader_in_use, projection, camera.ViewMatrix(), model);
  translate.Translate(0.0, -5.0f, 0.0f);
  scale.Scale(20.0f, 20.0f, 20.0f);
  model = translate * scale;
  MeshRenderer::Render(plane_mesh_id, shader_in_use, projection, camera.ViewMatrix(), model);
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