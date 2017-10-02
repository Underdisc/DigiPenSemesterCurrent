/* All content(c) 2016 - 2017 DigiPen(USA) Corporation, all rights reserved. */
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
  }
  GLuint APosition;
  GLuint UProjection;
  GLuint UView;
  GLuint UModel;
  GLuint ULineColor;
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
    ULightPosition = GetUniformLocation("ULightPosition");
    UAmbientFactor = GetUniformLocation("UAmbientFactor");
    UDiffuseFactor = GetUniformLocation("UDiffuseFactor");
    USpecularFactor = GetUniformLocation("USpecularFactor");
    USpecularExponent = GetUniformLocation("USpecularExponent");
    UAmbientColor = GetUniformLocation("UAmbientColor");
    UDiffuseColor = GetUniformLocation("UDiffuseColor");
    USpecularColor = GetUniformLocation("USpecularColor");
  }
  GLuint APosition;
  GLuint ANormal;
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
