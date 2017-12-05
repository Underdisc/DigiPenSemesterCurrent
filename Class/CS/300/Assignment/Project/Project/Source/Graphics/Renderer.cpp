/* All content(c) 2017 - 2018 DigiPen(USA) Corporation, all rights reserved. */
#include "Renderer.h"

void Renderer::AddRenderable(MeshRenderer::MeshObject * renderable)
{
  _renderables.insert({renderable, Math::Matrix4()});
}

Math::Matrix4 * Renderer::GetRenderableMatrix(
  MeshRenderer::MeshObject * renderable)
{
  std::unordered_map<MeshRenderer::MeshObject *, Math::Matrix4>::iterator
    renderable_location;
  renderable_location = _renderables.find(renderable);
  return &renderable_location->second;
}