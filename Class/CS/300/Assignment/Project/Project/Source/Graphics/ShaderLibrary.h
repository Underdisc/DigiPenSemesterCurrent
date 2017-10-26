/* All content(c) 2017 DigiPen(USA) Corporation, all rights reserved. */
/*****************************************************************************/
/*!
\file ShaderLibrary.h
\author Connor Deakin
\par E-mail: connor.deakin\@digipen.edu
\par Project: CS 300
\date 02/10/2017
\brief
  A library of shaders that can be used for different drawing techniques.
*/
/*****************************************************************************/
#pragma once

#include "Shader.h"

#define MAXLIGHTS 10

/*****************************************************************************/
/*!
\class LineShader
\brief
  Used for drawing single lines.
*/
/*****************************************************************************/
class LineShader : public Shader
{
public:
  LineShader();
  // Attributes
  GLuint APosition;
  // Uniforms
  GLuint UProjection;
  GLuint UView;
  GLuint UModel;
  GLuint ULineColor;
};



class SolidShader : public Shader
{
public:
  void EnableAttributes();
  void DisableAttributes();
public:
  SolidShader();
  // Attributes
  GLuint APosition;
  // Uniforms
  GLuint UProjection;
  GLuint UView;
  GLuint UModel;
  GLuint UColor;
};

/*****************************************************************************/
/*!
\class PhongShader
\brief
  Used for drawing objects that undergo phong shading. Supports lighting
  with up to 10 lights.
*/
/*****************************************************************************/
class PhongShader : public Shader
{
public:
  struct Material
  {
    GLuint UColor;
    GLuint UAmbientFactor;
    GLuint UDiffuseFactor;
    GLuint USpecularFactor;
    GLuint USpecularExponent;
  };
  struct Light
  {
    GLuint UType;
    GLuint UPosition;
    GLuint UDirection;
    GLuint UInnerAngle;
    GLuint UOuterAngle;
    GLuint UAmbientColor;
    GLuint UDiffuseColor;
    GLuint USpecularColor;
    GLuint UAttenuationC0;
    GLuint UAttenuationC1;
    GLuint UAttenuationC2;
  };
public:
  PhongShader();
  void EnableAttributes();
  void DisableAttributes();
  // Attributes
  GLuint APosition;
  GLuint ANormal;
  // Uniforms
  GLuint UProjection;
  GLuint UView;
  GLuint UModel;
  GLuint UCameraPosition;
  // Material Uniform
  Material UMaterial;
  // Light Uniforms
  GLuint UActiveLights;
  Light ULights[MAXLIGHTS];
  // Fog Uniforms
  GLuint UFogColor;
  GLuint UNearPlane;
  GLuint UFarPlane;
};

/*****************************************************************************/
/*!
\class GouraudShader
\brief
  Used for drawing objects that undergo gouraud shading. Can be used for
  shading with up to 10 lights.
*/
/*****************************************************************************/
class GouraudShader : public Shader
{
  struct Light
  {
    GLuint UPosition;
    GLuint UAmbientFactor;
    GLuint UDiffuseFactor;
    GLuint USpecularFactor;
    GLuint USpecularExponent;
    GLuint UAmbientColor;
    GLuint UDiffuseColor;
    GLuint USpecularColor;
  };
public:
  GouraudShader();
  void EnableAttributes();
  void DisableAttributes();
  // Attributes
  GLuint APosition;
  GLuint ANormal;
  // Uniforms
  GLuint UProjection;
  GLuint UView;
  GLuint UModel;
  GLuint UObjectColor;
  // Uniform array for lights
  GLuint UActiveLights;
  Light ULights[MAXLIGHTS];
};

/*****************************************************************************/
/*!
\class TextureShader
\brief
  Used for displaying a single texture on a quad.
*/
/*****************************************************************************/
class TextureShader : public Shader
{
public:
  TextureShader();
  // Attributes
  GLuint APosition;
  GLuint ATexCoord;
  // Uniforms
  GLuint UProjection;
  GLuint UView;
  GLuint UModel;
  GLuint UTexture;
};
