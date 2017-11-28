#pragma once

#define MAPSPHERICAL 0
#define MAPCYLINDRICAL 1
#define MAPPLANAR 2

#include "Color.h"
#include "Shader/ShaderLibrary.h"

struct Material
{
  Material();
  void SetUniforms(PhongShader * phong_shader);
  void SetUniforms(GouraudShader * gouraud_shader);
  void SetUniforms(BlinnShader * blinn_shader);
  Color _color;
  float _ambientFactor;
  float _diffuseFactor;
  float _specularFactor;
  float _specularExponent;
  bool _textureMapping;
  bool _specularMapping;
  bool _normalMapping;
  int _mappingType;
  int _diffuseMap;
  int _specularMap;
  int _normalMap;
};