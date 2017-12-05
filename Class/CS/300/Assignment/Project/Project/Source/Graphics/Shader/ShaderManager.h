#include "ShaderLibrary.h"

// for future reference
// only the renderer should have this
// not the mesh renderer
class ShaderManager
{
public:
  static void Initialize();
  static void Purge();
public:
  static SkyboxShader * _skybox;
private:
  ShaderManager();
};

SkyboxShader * ShaderManager::_skybox = nullptr;

void ShaderManager::Initialize()
{
  _skybox = new SkyboxShader();
}

void ShaderManager::Purge()
{
  _skybox->Purge();
  delete _skybox;
}
