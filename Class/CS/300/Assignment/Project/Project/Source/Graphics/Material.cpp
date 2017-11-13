#include "Material.h"

Material::Material() : _color(1.0f, 1.0f, 1.0f), _ambientFactor(0.1f),
_diffuseFactor(1.0f), _specularFactor(1.0f), _specularExponent(3.0f)
{}
void Material::SetUniforms(PhongShader * phong_shader)
{
  glUniform3f(phong_shader->UMaterial.UColor,
    _color._r, _color._g, _color._b);
  glUniform1f(phong_shader->UMaterial.UAmbientFactor, _ambientFactor);
  glUniform1f(phong_shader->UMaterial.UDiffuseFactor, _diffuseFactor);
  glUniform1f(phong_shader->UMaterial.USpecularFactor, _specularFactor);
  glUniform1f(phong_shader->UMaterial.USpecularExponent, _specularExponent);
}
void Material::SetUniforms(GouraudShader * gouraud_shader)
{
  glUniform3f(gouraud_shader->UMaterial.UColor,
    _color._r, _color._g, _color._b);
  glUniform1f(gouraud_shader->UMaterial.UAmbientFactor, _ambientFactor);
  glUniform1f(gouraud_shader->UMaterial.UDiffuseFactor, _diffuseFactor);
  glUniform1f(gouraud_shader->UMaterial.USpecularFactor, _specularFactor);
  glUniform1f(gouraud_shader->UMaterial.USpecularExponent, _specularExponent);
}
void Material::SetUniforms(BlinnShader * blinn_shader)
{
  glUniform3f(blinn_shader->UMaterial.UColor,
    _color._r, _color._g, _color._b);
  glUniform1f(blinn_shader->UMaterial.UAmbientFactor, _ambientFactor);
  glUniform1f(blinn_shader->UMaterial.UDiffuseFactor, _diffuseFactor);
  glUniform1f(blinn_shader->UMaterial.USpecularFactor, _specularFactor);
  glUniform1f(blinn_shader->UMaterial.USpecularExponent, _specularExponent);
}