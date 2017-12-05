/* All content(c) 2017 - 2018 DigiPen(USA) Corporation, all rights reserved. */
#pragma once

#include <unordered_map>
#include "Mesh/MeshRenderer.h"
#include "../Math/Matrix4.h"

class Renderer
{
public:
  void Render(const Math::Matrix4 & projection, const Math::Matrix4 & view);
  void AddRenderable(MeshRenderer::MeshObject * renderable);
  Math::Matrix4 * GetRenderableMatrix(MeshRenderer::MeshObject * renderable);
private:
  Renderer();
  std::unordered_map<MeshRenderer::MeshObject *, Math::Matrix4> _renderables; 

};