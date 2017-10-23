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
  LineShader() :
    Shader("Resource/Shader/line.vert", "Resource/Shader/line.frag")
  {
    APosition = GetAttribLocation("APosition");
    UProjection = GetUniformLocation("UProjection");
    UView = GetUniformLocation("UView");
    UModel = GetUniformLocation("UModel");
    ULineColor = GetUniformLocation("ULineColor");
    UTime = GetUniformLocation("UTime");
  }
  // Attributes
  GLuint APosition;
  // Uniforms
  GLuint UProjection;
  GLuint UView;
  GLuint UModel;
  GLuint ULineColor;
  GLuint UTime;
};

/*****************************************************************************/
/*!
\class PhongShader
\brief
  Used for drawing objects that undergo phong shader. Currently only usable
  with one light. Multiple light support will be added in the future.
*/
/*****************************************************************************/
class PhongShader : public Shader
{
public:
  PhongShader() :
    Shader("Resource/Shader/phong.vert", "Resource/Shader/phong.frag")
  {
    APosition = GetAttribLocation("APosition");
    ANormal = GetAttribLocation("ANormal");
    UProjection = GetUniformLocation("UProjection");
    UView = GetUniformLocation("UView");
    UModel = GetUniformLocation("UModel");
    UObjectColor = GetUniformLocation("UObjectColor");
    ULightPosition = GetUniformLocation("light._position");
    UAmbientFactor = GetUniformLocation("light._ambientFactor");
    UDiffuseFactor = GetUniformLocation("light._diffuseFactor");
    USpecularFactor = GetUniformLocation("light._specularFactor");
    USpecularExponent = GetUniformLocation("light._specularExponent");
    UAmbientColor = GetUniformLocation("light._ambientColor");
    UDiffuseColor = GetUniformLocation("light._diffuseColor");
    USpecularColor = GetUniformLocation("light._specularColor");
  }
  // Attributes
  GLuint APosition;
  GLuint ANormal;
  // Uniforms
  GLuint UProjection;
  GLuint UView;
  GLuint UModel;
  GLuint UObjectColor;
  GLuint ULightPosition;
  GLuint UAmbientFactor;
  GLuint UDiffuseFactor;
  GLuint USpecularFactor;
  GLuint USpecularExponent;
  GLuint UAmbientColor;
  GLuint UDiffuseColor;
  GLuint USpecularColor;
};


class TextureShader : public Shader
{
public:
  TextureShader() : 
  Shader("Resource/Shader/texture.vert", "Resource/Shader/texture.frag")
  {
    APosition = GetAttribLocation("APosition");
    ATexCoord = GetAttribLocation("ATexCoord");
    UProjection = GetUniformLocation("UProjection");
    UView = GetUniformLocation("UView");
    UModel = GetUniformLocation("UModel");
    UTexture = GetUniformLocation("UTexture");
  }
  GLuint APosition;
  GLuint ATexCoord;
  GLuint UProjection;
  GLuint UView;
  GLuint UModel;
  GLuint UTexture;
};
