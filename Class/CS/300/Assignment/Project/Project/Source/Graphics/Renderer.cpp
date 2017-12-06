/* All content(c) 2017 - 2018 DigiPen(USA) Corporation, all rights reserved. */
#include "Renderer.h"

// static initializations
Mesh * Renderer::_mesh = nullptr;
MeshRenderer::MeshObject * Renderer::_meshObject = nullptr;
MeshRenderer::MeshObject * Renderer::_sphereMeshObject = nullptr;
MeshRenderer::MeshObject * Renderer::_planeMeshObject = nullptr;

TextureObject * Renderer::_diffuseTextureObject = nullptr;
TextureObject * Renderer::_specularTextureObject = nullptr;
TextureObject * Renderer::_normalTextureObject = nullptr;

Skybox * Renderer::_skybox = nullptr;

void Renderer::Initialize() 
{

}