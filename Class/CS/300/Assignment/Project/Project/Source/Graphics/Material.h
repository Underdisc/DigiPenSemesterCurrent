#pragma once

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
};