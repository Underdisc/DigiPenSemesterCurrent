#pragma once

#include <string>
#include "../Graphics/Mesh/Mesh.h"
#include "../Graphics/Mesh/MeshRenderer.h"
#include "../Graphics/Light.h"
#include "../Graphics/Material.h"
#include "../Graphics/Camera.h"
#include "../Math/Vector3.h"
#include "../Math/EulerAngles.h"


#define PI 3.141592653589f
#define PI2 6.28318530718f
#define FPS 60
#define MODEL_PATH "Resource/Model/"
#define FILENAME_BUFFERSIZE 50

class Editor
{
// editor windows
public:
  static void Update(Mesh * mesh, MeshRenderer::MeshObject * mesh_object,
    void (*load_mesh)(const std::string &));
  static void MaterialEditorUpdate();
  static void LightEditorUpdate();
public:
  static void ManageInput();
public:
  static void SceneMix();
  static void SceneSame();
public:
  static bool show_light_editor;
  static bool show_material_editor;

  static std::string current_mesh;
  static char next_mesh[FILENAME_BUFFERSIZE];
  static Light lights[MAXLIGHTS];
  static Material material;
  static unsigned int active_lights;
  static MeshRenderer::ShaderType shader_in_use;
  static bool rotating_lights;
  static float rotate_light_speed;

  static bool rotate_camera;
  static float camera_rotate_speed;
  static float camera_distance;
  static Math::Vector3 trans;
  static float cur_scale;
  static float scale_speed;
   // initializing to pie so rotations make sense at the start
  static Math::EulerAngles rotation;
private:
  Editor();
};