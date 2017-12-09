/* All content(c) 2017 - 2018 DigiPen(USA) Corporation, all rights reserved. */

#include "Mesh/Mesh.h"
#include "Mesh/MeshRenderer.h"
#include "Texture/TexturePool.h"
#include "Framebuffer.h"
#include "Skybox.h"
#include "Camera.h"

#ifndef RENDERER_H
#define RENDERER_H


class Renderer
{
public:
  static void Initialize(Mesh & mesh);
  static void Purge();
  static void Clear();
  static void RenderEnvironment();
  static void Render(const Math::Matrix4 & projection,
    const Math::Matrix4 & view, const Math::Vector3 & position, bool mesh);
  static void ReplaceMesh(Mesh & mesh);
public:
  static Mesh * _mesh;
  static MeshRenderer::MeshObject * _meshObject;
  static MeshRenderer::MeshObject * _sphereMeshObject;
  static MeshRenderer::MeshObject * _planeMeshObject;

  static TextureObject * _diffuseTextureObject;
  static TextureObject * _specularTextureObject;
  static TextureObject * _normalTextureObject;

  static Skybox * _skybox;

  static Framebuffer _fbUp;
  static Framebuffer _fbDown;
  static Framebuffer _fbLeft;
  static Framebuffer _fbRight;
  static Framebuffer _fbFront;
  static Framebuffer _fbBack;

};

#endif