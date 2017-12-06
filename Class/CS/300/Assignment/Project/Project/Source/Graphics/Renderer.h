/* All content(c) 2017 - 2018 DigiPen(USA) Corporation, all rights reserved. */

#include "Mesh/Mesh.h"
#include "Mesh/MeshRenderer.h"
#include "Texture/TexturePool.h"
#include "Skybox.h"
#include "Camera.h"

#ifndef RENDERER_H
#define RENDERER_H

class Renderer
{
public:
  static void Initialize(Mesh & mesh);
  static void Purge();
  static void Render(const Math::Matrix4 & projection, Camera * camera);
public:
  static Mesh * _mesh;
  static MeshRenderer::MeshObject * _meshObject;
  static MeshRenderer::MeshObject * _sphereMeshObject;
  static MeshRenderer::MeshObject * _planeMeshObject;

  static TextureObject * _diffuseTextureObject;
  static TextureObject * _specularTextureObject;
  static TextureObject * _normalTextureObject;

  static Skybox * _skybox;

};

#endif